/*
** Demo/skeleton code for CMSC 23700 Project 2
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "spot.h"

/*
** Prior to including glfw.h: sneakily #define the "__gl_h_" include
** guard of OpenGL/gl.h so that the "#include <OpenGL/gl.h>" in glfw.h
** will have no effect.
*/
#define __gl_h_
#define GLFW_NO_GLU /* Also, tell glfw.h not to include GLU header */
#include <GL/glfw.h>
#undef GLFW_NO_GLU
#undef __gl_h_

#include <AntTweakBar.h>


/*
** The camera_t is a suggested storage place for all the parameters associated
** with determining how you see the scene, which is used to determine one of
** the transforms applied in the vertex shader.  Right now there are no helper
** functions to initialize or compute with the camera_t; that is up to you.
**
*/
typedef struct {
  GLfloat from[3],    /* location (in world-space) of eyepoint */
    at[3],            /* what point (in world-space) we are looking at */
    up[3],            /* what is up direction for eye (this is not updated to
                         the "true" up) */
    aspect,           /* the ratio of horizontal to vertical size of the view
                         window */
    fov,              /* The angle, in degrees, vertically subtended by the
                         near clipping plane */
    near, far;        /* near and far clipping plane distances.  Whether you
                         interpret these as relative to the eye "from" point
                         (the convention in graphics) or relative to the 
                         "at" point (arguably more convenient) is up to you */
  int ortho,          /* (a boolean) no perspective projection: just
                         orthographic */
    upfixed;          /* up vector stays fixed during camera rotations */
} camera_t;

/*
** A string to use as title bar name for the "tweak bar"
*/
#define TBAR_NAME "Project2-Params"

/*
** The uniloc_t is a possible place to store "locations" of shader
** uniform variables, so they can be learned once and re-used once per
** render.  Modify as you see fit! 
*/
typedef struct {
  GLint modelMatrix;  /* same name as field in spotGeom */
  GLint normalMatrix; /* same name as field in spotGeom */
  GLint objColor;     /* same name as field in spotGeom */
  GLint Ka;           /* same name as field in spotGeom */
  GLint Kd;           /* same name as field in spotGeom */
  GLint Ks;           /* same name as field in spotGeom */
  GLint shexp;        /* same name as field in spotGeom */
  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  GLint viewMatrix;   /* possible name of view matrix in vertex shader */
  GLint projMatrix;   /* possible name of projection matrix in vertex shader */
  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
  GLint lightDir;     /* same name as field in context_t */
  GLint samplerA;     /* possible name of texture sampler in fragment shader */
  GLint samplerB;     /* possible name of texture sampler in fragment shader */
} uniloc_t;

/*
** The context_t is a suggested storage place for what might otherwise be
** separate global variables (globals obscure the flow of information and are
** hence bad style).  Modify as you see fit!
**
*/
typedef struct {
  const char *vertFname, /* file name of vertex shader */
    *fragFname;          /* file name of fragment shader */
  spotGeom **geom;       /* array of spotGeom's to render */
  unsigned int geomNum;  /* length of geom */
  spotImage **image;     /* array of texture images to use */
  unsigned int imageNum; /* length of image */
  GLfloat bgColor[3];  /* background color */
  GLfloat lightDir[3];   /* direction pointing to light (at infinity) */
  int running;           /* we exit when this is zero */
  GLint program;         /* the linked shader program */
  int winSizeX, winSizeY; /* size of rendering window */
  int tbarSizeX,         /* initial width of tweak bar */
    tbarSizeY,           /* initial height of tweak bar */
    tbarMargin;          /* margin between tweak bar and window */

  camera_t camera;       /* a camera */
  uniloc_t uniloc;       /* store of uniform locations */
  int lastX, lastY;      /* coordinates of last known mouse position */
  int buttonDown,        /* mouse button is being held down */
    shiftDown;           /* shift was down at time of mouse click */
  TwBar *tbar;           /* pointer to the parameter "tweak bar" */
  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  /* (any other information about the state of mouse or keyboard
     input, geometry, camera, transforms, or anything else that may
     need to be accessed from anywhere */
  GLfloat vspNear, vspFar,  U[3], V[3], N[3];
  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
} context_t;

/****
***** The GLFW callbacks (e.g. callbackResize) don't take additional
***** arguments, so we use one (and only one) global variable, of type
***** context_t.
****/
context_t *gctx = NULL;
/****
*****
*****
****/


/* Creates a context around geomNum spotGeom's and
   imageNum spotImage's */
context_t *contextNew(unsigned int geomNum, unsigned int imageNum) {
  const char me[]="contextNew";
  context_t *ctx;
  unsigned int gi;
  
  ctx = (context_t *)calloc(1, sizeof(context_t));
  if (!ctx) {
    spotErrorAdd("%s: couldn't alloc context?", me);
    return NULL;
  }

  ctx->vertFname = NULL;
  ctx->fragFname = NULL;
  if (geomNum) {
    ctx->geom = (spotGeom **)calloc(geomNum, sizeof(spotGeom*));
    if (!ctx->geom) {
      spotErrorAdd("%s: couldn't alloc %u geoms", me, geomNum);
      free(ctx); return NULL;
    }
    for (gi=0; gi<geomNum; gi++) {
      ctx->geom[gi] = NULL;
    }
  } else {
    ctx->geom = NULL;
  }
  ctx->geomNum = geomNum;
  if (imageNum) {
    ctx->image = (spotImage **)calloc(imageNum, sizeof(spotImage*));
    if (!ctx->image) {
      spotErrorAdd("%s: couldn't alloc %u images", me, imageNum);
      free(ctx); return NULL;
    }
    for (gi=0; gi<imageNum; gi++) {
      ctx->image[gi] = spotImageNew();
    }
  } else {
    ctx->image = NULL;
  }
  ctx->imageNum = imageNum;
  SPOT_V3_SET(ctx->bgColor, 0.2f, 0.25f, 0.3f);
  SPOT_V3_SET(ctx->lightDir, 1.0f, 0.0f, 0.0f);
  ctx->running = 1; /* non-zero == true */
  ctx->program = 0;
  ctx->winSizeX = 900;
  ctx->winSizeY = 700;
  ctx->tbarSizeX = 200;
  ctx->tbarSizeY = 300;
  ctx->tbarMargin = 20;
  ctx->lastX = ctx->lastY = -1;
  ctx->buttonDown = 0;
  ctx->shiftDown = 0;

  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  /* good place to initialize camera */

  if (2 == geomNum) {
    ctx->geom[0] = spotGeomNewSphere();
    ctx->geom[1] = spotGeomNewSquare();
    SPOT_M4_SET(ctx->geom[1]->modelMatrix,
                2.0, 0.0, 0.0, 0.0,
                0.0, 2.0, 0.0, 0.0,
                0.0, 0.0, 2.0,-1.0,
                0.0, 0.0, 0.0, 1);
  }

  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
  return ctx;
}

int contextGLInit(context_t *ctx) {
  const char me[]="contextGLInit";
  unsigned int ii;

  /* solid, not wireframe or points */
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  /* No backface culling for now */
  glDisable(GL_CULL_FACE);
  /* Yes, do depth testing */
  glEnable(GL_DEPTH_TEST);

  /* Create shader program.  Note that the names of per-vertex attributes
     are specified here.  This includes  vertPos and vertNorm from last project
     as well as new vertTex2 (u,v) per-vertex texture coordinates, and the
     vertTang per-vertex surface tangent 3-vector. */
  ctx->program = spotProgramNew(ctx->vertFname, ctx->fragFname,
                                "vertPos", spotVertAttrIndx_xyz,
                                "vertNorm", spotVertAttrIndx_norm,
                                "vertTex2", spotVertAttrIndx_tex2,
                                "vertTang", spotVertAttrIndx_tang,
                                /* input name, attribute index pairs
                                   MUST BE TERMINATED with NULL */
                                NULL);
  if (!ctx->program) {
    spotErrorAdd("%s: couldn't create shader program", me);
    return 1;
  }
  
  /* Learn (once) locations of uniform variables that we will
     frequently set */
#define SET_UNILOC(V) ctx->uniloc.V = glGetUniformLocation(ctx->program, #V)
  
  SET_UNILOC(lightDir);
  SET_UNILOC(modelMatrix);
  SET_UNILOC(normalMatrix);
  SET_UNILOC(viewMatrix);
  SET_UNILOC(projMatrix);
  SET_UNILOC(objColor);
  SET_UNILOC(Ka);
  SET_UNILOC(Kd);
  SET_UNILOC(samplerA);
  SET_UNILOC(samplerB);
  
#undef SET_UNILOC;
  
  if (ctx->geom) {
    for (ii=0; ii<ctx->geomNum; ii++) {
      if (spotGeomGLInit(ctx->geom[ii])) {
        spotErrorAdd("%s: trouble with geom[%u]", me, ii);
        return 1;
      }
    }
  }
  if (ctx->image) {
    for (ii=0; ii<ctx->imageNum; ii++) {
      if (ctx->image[ii]->data.v) {
        /* only bother with GL init when image data has been set */
        if (spotImageGLInit(ctx->image[ii])) {
          spotErrorAdd("%s: trouble with image[%u]", me, ii);
          return 1;
        }
      }
    }
  }

  return 0;
}

int contextGLDone(context_t *ctx) {
  const char me[]="contextGLDone";
  unsigned int ii;

  if (!ctx) {
    spotErrorAdd("%s: got NULL pointer", me);
    return 1;
  }
  if (ctx->geom) {
    for (ii=0; ii<ctx->geomNum; ii++) {
      spotGeomGLDone(ctx->geom[ii]);
    }
  }
  if (ctx->image) {
    for (ii=0; ii<ctx->imageNum; ii++) {
      if (ctx->image[ii]->data.v) {
        spotImageGLDone(ctx->image[ii]);
      }
    }
  }
  return 0;
}

/*
** Anything that is dynamically (at run-time) allocated in contextNew()
** should be cleaned up here
*/
context_t *contextNix(context_t *ctx) {
  unsigned int ii;

  if (!ctx) {
    return NULL;
  }
  if (ctx->geom) {
    for (ii=0; ii<ctx->geomNum; ii++) {
      spotGeomNix(ctx->geom[ii]);
    }
    free(ctx->geom);
  }
  if (ctx->image) {
    for (ii=0; ii<ctx->imageNum; ii++) {
      spotImageNix(ctx->image[ii]);
    }
    free(ctx->image);
  }
  free(ctx);
  return NULL;
}

/* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
/* 
   (other functions to act on the context_t; such as dealing with
   viewpoint changes 
*/

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

int contextDraw(context_t *ctx) {
  const char me[]="contextDraw";
  GLfloat demoView[16], demoProj[16];
  unsigned int gi;

  /* re-assert which program is being used (AntTweakBar uses its own) */
  glUseProgram(ctx->program); 

  /* background color; setting alpha=0 means that we'll see the
     background color in the render window, but upon doing
     "spotImageScreenshot(img, SPOT_TRUE)" (SPOT_TRUE for "withAlpha")
     we'll get a meaningful alpha channel, so that the image can
     recomposited with a different background, or used in programs
     (including web browsers) that respect the alpha channel */
  glClearColor(ctx->bgColor[0], ctx->bgColor[1], ctx->bgColor[2], 0.0f);
  /* Clear the window and the depth buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */

  /* The following will be useful when you want to use textures,
     especially two textures at once, here sampled in the fragment
     shader with "samplerA" and "samplerB".  There are some
     non-intuitive calls required to specify which texture data will
     be sampled by which sampler.  See OpenGL SuperBible (5th edition)
     pg 279.  Also, http://tinyurl.com/7bvnej3 is amusing and
     informative */
  /*
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ctx->image[0]->textureId);
  glUniform1i(ctx->uniloc.samplerA, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, ctx->image[1]->textureId);
  glUniform1i(ctx->uniloc.samplerB, 1);
  */

  SPOT_M4_SET(demoView,
              -0.4423201,      0.8968572,       0.0000000,     -0.0046070,
              -0.4998170,     -0.2465043,       0.8303122,      0.3545303,
              -0.7446715,     -0.3672638,      -0.5572984,      9.9955845,
              0.0000000,       0.0000000,       0.0000000,      1.0000000);
  SPOT_M4_SET(demoProj,
              4.8082800,       0.0000000,       0.0000000,       0.0000000,
              0.0000000,       6.1820741,       0.0000000,       0.0000000,
              0.0000000,       0.0000000,       3.0375593,     -27.0835247,
              0.0000000,       0.0000000,       1.0000000,       0.0000000);

  glUniformMatrix4fv(ctx->uniloc.viewMatrix, 1, GL_FALSE, demoView);
  glUniformMatrix4fv(ctx->uniloc.projMatrix, 1, GL_FALSE, demoProj);
  glUniform3fv(ctx->uniloc.lightDir, 1, ctx->lightDir);
  for (gi=0; gi<ctx->geomNum; gi++) {
    glUniformMatrix4fv(ctx->uniloc.modelMatrix, 
                       1, GL_FALSE, ctx->geom[gi]->modelMatrix);
    glUniformMatrix3fv(ctx->uniloc.normalMatrix,
                       1, GL_FALSE, ctx->geom[gi]->normalMatrix);
    glUniform3fv(ctx->uniloc.objColor, 1, ctx->geom[gi]->objColor);
    glUniform1f(ctx->uniloc.Ka, ctx->geom[gi]->Ka);
    glUniform1f(ctx->uniloc.Kd, ctx->geom[gi]->Kd);
    spotGeomDraw(ctx->geom[gi]);
  }
  
  /* These lines are also related to using textures.  We finish by
     leaving GL_TEXTURE0 as the active unit since AntTweakBar uses
     that, but doesn't seem to explicitly select it */
  /*
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  */

  /* You are welcome to do error-checking with higher granularity than
     just once per render, in which case this error checking loop
     should be repackaged into its own function. */
  GLenum glerr = glGetError();
  if (glerr) {
    while (glerr) {
      spotErrorAdd("%s: OpenGL error %d (%s)",
                   me, glerr, spotGLErrorString(glerr));
      glerr = glGetError();
    }
    return 1;
  }
  /* else */
  return 0;
}

void callbackKeyboard(int key, int action) {
  const char me[]="callbackKeyboard";

  /* give AntTweakBar first pass at handling with key event */
  if (TwEventKeyGLFW(key, action)) {
    /* the event was handled by AntTweakBar; nothing more for us to do */
    return;
  }

  if (action != GLFW_PRESS) {
    /* only acting on key press, not release */
    return;
  }

  if ('D' == key) {
    FILE *file;
    spotImage *shot;
    int test, testMax=99999;
    char fname[128]; /* long enough to hold filename */

    shot = spotImageNew();
    /* copy image from render window; last argument controls whether
       image is retreived with (SPOT_TRUE) or without (SPOT_FALSE) an
       alpha channel */
    if (spotImageScreenshot(shot, SPOT_TRUE)) {
      fprintf(stderr, "%s: trouble getting image:\n", me);
      spotErrorPrint(); spotErrorClear();
      return;
    }
    /* find unused filename */
    for (test=0, file=NULL; test<=testMax; test++) {
      /* feel free to change the filename format used here! */
      sprintf(fname, "%05d.png", test);
      if (!(file = fopen(fname, "rb"))) {
        /* couldn't open fname => it didn't exist => we can use fname, done */
        break;
      }
      /* else we *could* open it => already used => close it try again */
      fclose(file);
      file = NULL;
    }
    if (test > testMax) {
      fprintf(stderr, "%s: unable to find unused filename to write to!", me);
      spotImageNix(shot);
      return;
    }
    /* save image */
    if (spotImageSavePNG(fname, shot)) {
      fprintf(stderr, "%s: trouble saving to %s:\n", me, fname);
      spotImageNix(shot); spotErrorPrint(); spotErrorClear();
      return;
    }
    spotImageNix(shot);
    return;
  }

  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  /* (process keyboard input */
  if ('Q' == key) {
    gctx->running = 0;
    return;
  }
  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
  return;
}

void callbackMousePos(int xx, int yy) {
  /* const char me[]="callbackMousePos"; */

  if (!gctx->buttonDown) {
    /* nothing for us to do, but AntTweakBar might care about the event */
    TwEventMousePosGLFW(xx, yy);
    /* we return regardless of whether AntTweakBar handled it because
       we only do things when the button is pressed */
    return;
  }
  /* else, we know gctx->buttonDown is set, which only happens when we
     (not AntTweakBar) handled the button press event, so there is no
     reason to give TwEventMousePosGLFW another chance at handling it */

  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */

  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
  gctx->lastX = xx;
  gctx->lastY = yy;
  return;
}

#define FIFTH 0.2
void callbackMouseButton(int button, int action) {
  /* const char me[]="callbackMouseButton"; */

  /* give AntTweakBar first pass at handling mouse event */
  if (TwEventMouseButtonGLFW(button, action)) {
    /* AntTweakBar has handled event, nothing more for us to do,
       not even recording buttonDown */
    return;
  }

  if (GLFW_PRESS == action) {
    int xx, yy;
    float xf, yf;
    gctx->buttonDown = 1;
    if (GLFW_PRESS == glfwGetKey(GLFW_KEY_LSHIFT)
        || GLFW_PRESS == glfwGetKey(GLFW_KEY_RSHIFT)) {
      gctx->shiftDown = 1;
    } else {
      gctx->shiftDown = 0;
    }
    /* determine action according to position of button press */
    glfwGetMousePos(&xx, &yy);
    xf = (float)xx/gctx->winSizeX;
    yf = (float)yy/gctx->winSizeY;
    /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
    if (yf > 1 - FIFTH) {

    } else if (xf < FIFTH) {

    } else {

    }
    /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
    /* Save position to permit tracking of changes in position. */
    gctx->lastX = xx;
    gctx->lastY = yy;
  } else {
    /* The button has been released. */
    gctx->buttonDown = 0;
    gctx->shiftDown = 0;
  }
  return;
}

void callbackResize(int w, int h) {
  const char me[]="callbackResize";
  char buff[128];

  if (h == 0) {
    h = 1;
  }
  /* record new window dimensions */
  gctx->winSizeX = w;
  gctx->winSizeY = h;
  /* Set OpenGL viewport to window dimensions */
  glViewport(0, 0, w, h);
  /* let AntTweakBar know about new window dimensions */
  TwWindowSize(w, h);
  
  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  /* (deal with window resizing; note that the *vertical* FOV should
     be maintained throughout a resize) */ 


  /* By default the tweak bar maintains its position relative to the
     LEFT edge of the window, which we are using for camera control.
     So, move the tweak bar to a new position, fixed relative to RIGHT
     edge of the window. You can remove/modify this based on your
     preferences. */
  sprintf(buff, TBAR_NAME " position='%d %d' ",
          gctx->winSizeX - gctx->tbarSizeX - gctx->tbarMargin,
          gctx->tbarMargin);
  TwDefine(buff);
  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

  /* redraw both the context and the tweak bar with new window size to
     give visual feedback during resizing */
  if (contextDraw(gctx)) {
    fprintf(stderr, "%s: trouble drawing during resize:\n", me);
    spotErrorPrint(); spotErrorClear();
    gctx->running = 0;
  }
  if (!TwDraw()) {
    fprintf(stderr, "%s: AntTweakBar error: %s\n", me, TwGetLastError());
    gctx->running = 0;
  }
  glfwSwapBuffers();
  return;
}

int createTweakBar(context_t *ctx) {
  const char me[]="createTweakBar";
  char buff[128];
  int EE;  /* we have an error */
  
  EE = 0;
  /* Create a tweak bar for interactive parameter adjustment */
  if (!EE) EE |= !(ctx->tbar = TwNewBar(TBAR_NAME));
  /* documentation for the TwDefine parameter strings here:
     http://www.antisphere.com/Wiki/tools:anttweakbar:twbarparamsyntax */
  /* add a message to be seen in the "help" window */
  if (!EE) EE |= !TwDefine(" GLOBAL help='This description of Project 2 "
                           "has not been changed by anyone but students "
                           "are encouraged to write something descriptive "
                           "here.' ");
  /* change location where bar will be drawn, over to the right some
     to expose more of the left edge of window.  Note that we are
     exploiting the automatic compile-time concatentation of strings
     in C, which connects TBAR_NAME with the rest of the string to
     make one contiguous string */
  sprintf(buff, TBAR_NAME " position='%d %d' ",
          ctx->winSizeX - ctx->tbarSizeX - ctx->tbarMargin,
          ctx->tbarMargin);
  if (!EE) EE |= !TwDefine(buff);
  /* adjust other aspects of the bar */
  sprintf(buff, TBAR_NAME " color='0 0 0' alpha=10 size='%d %d' ",
          ctx->tbarSizeX, ctx->tbarSizeY);
  if (!EE) EE |= !TwDefine(buff);
  
  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  /* Add definitions of the variables that we want to tweak */

  if (!EE) EE |= !TwAddVarRW(ctx->tbar, "Ka",
                             TW_TYPE_FLOAT, &(ctx->geom[0]->Ka),
                             " label='Ka' min=0.0 max=1.0 step=0.005");
  if (!EE) EE |= !TwAddVarRW(ctx->tbar, "Kd",
                             TW_TYPE_FLOAT, &(ctx->geom[0]->Kd),
                             " label='Kd' min=0.0 max=1.0 step=0.005");
  if (!EE) EE |= !TwAddVarRW(ctx->tbar, "bgColor",
                             TW_TYPE_COLOR3F, &(ctx->bgColor),
                             " label='bkgr color' ");
  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

  /* see also:
     http://www.antisphere.com/Wiki/tools:anttweakbar:twtype
     http://www.antisphere.com/Wiki/tools:anttweakbar:twdefineenum
  */

  if (EE) {
    spotErrorAdd("%s: AntTweakBar initialization failed:\n      %s",
                 me, TwGetLastError());
    return 1;
  }
  return 0;
}

void usage(const char *me) {
  /*                 argv[0]     [1]           [2]     3 == argc */
  fprintf(stderr, "usage: %s <vertshader> <fragshader>\n", me);
}

int main(int argc, const char* argv[]) {
  const char *me;

  me = argv[0];
  if (3 != argc) {
    usage(me);
    exit(1);
  }

  /* vvvvvvvvvvvvvvvvvvvvv YOUR CODE HERE vvvvvvvvvvvvvvvvvvvvvvvv */
  if (!(gctx = contextNew(2, 0))) {
    fprintf(stderr, "%s: context set-up problem:\n", me);
    spotErrorPrint(); spotErrorClear();
    exit(1);
  }
  /* save shader filenames */
  gctx->vertFname = argv[1];
  gctx->fragFname = argv[2];
  /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    exit(1);
  }

  /* Make sure we're using OpenGL 3.2 core.  NOTE: Changing away from
     OpenGL 3.2 core is not needed and not allowed for this project */
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
  glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (!glfwOpenWindow(gctx->winSizeX, gctx->winSizeY,
                      0,0,0,0, 32,0, GLFW_WINDOW)) {
    fprintf(stderr, "Failed to open GLFW window\n");
    glfwTerminate();
    exit(1);
  }

  glfwSetWindowTitle("Project 2 Window Title (change me!)");
  glfwEnable(GLFW_MOUSE_CURSOR);
  glfwEnable(GLFW_KEY_REPEAT);
  glfwSwapInterval(1);

  /* Initialize AntTweakBar */
  if (!TwInit(TW_OPENGL_CORE, NULL)) {
    fprintf(stderr, "AntTweakBar initialization failed: %s\n",
            TwGetLastError());
    exit(1);
  }

  printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
  printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
  printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
  printf("PNG_LIBPNG_VER_STRING = %s\n", PNG_LIBPNG_VER_STRING);
  
  /* set-up and initialize the global context */
  if (contextGLInit(gctx)) {
    fprintf(stderr, "%s: context OpenGL set-up problem:\n", me);
    spotErrorPrint(); spotErrorClear();
    TwTerminate();
    glfwTerminate();
    exit(1);
  }

  if (createTweakBar(gctx)) {
    fprintf(stderr, "%s: AntTweakBar problem:\n", me);
    spotErrorPrint(); spotErrorClear();
    TwTerminate();
    glfwTerminate();
    exit(1);
  }

  glfwSetWindowSizeCallback(callbackResize);
  glfwSetKeyCallback(callbackKeyboard);
  glfwSetMousePosCallback(callbackMousePos);
  glfwSetMouseButtonCallback(callbackMouseButton);

  /* Redirect GLFW mouse wheel events directly to AntTweakBar */
  glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
  /* Redirect GLFW char events directly to AntTweakBar */
  glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);

  /* Main loop */
  while (gctx->running) {
    /* render */
    if (contextDraw(gctx)) {
      fprintf(stderr, "%s: trouble drawing:\n", me);
      spotErrorPrint(); spotErrorClear();
      /* Can comment out "break" so that OpenGL bugs are reported but
         do not lead to the termination of the program */
      /* break; */
    }
    /* Draw tweak bar last, just prior to buffer swap */
    if (!TwDraw()) {
      fprintf(stderr, "%s: AntTweakBar error: %s\n", me, TwGetLastError());
      break;
    }
    /* Display rendering results */
    glfwSwapBuffers();
    /* NOTE: don't call glfwWaitEvents() if you want to redraw continuously */
    glfwWaitEvents();
    /* quit if window was closed */
    if (!glfwGetWindowParam(GLFW_OPENED)) {
      gctx->running = 0;
    }
  }
  
  contextGLDone(gctx);
  contextNix(gctx);
  TwTerminate();
  glfwTerminate();

  exit(0);
}
