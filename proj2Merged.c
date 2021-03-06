/*
 * Name          | cNet ID
 * --------------|--------------
 * Shir Yehoshua | shiryehoshua
 * Mark Roberts  | andrus
 *
 * Please grade Mark's repository
 *
 */
/*
  proj1.c: Demo/skeleton code for CMSC 23700 Project 1
  Copyright (C) 2012  University of Chicago

  Credits: This code started as a heavily modified version of
  oglsuperbible5-read-only/Src/Chapter03/GeoTest/GoeTest.cpp
  via http://code.google.com/p/oglsuperbible5/source/checkout
  from the code distribution of "OpenGL SuperBible", by Richard S. Wright Jr.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#else
#  include <GL/gl3.h>
#endif
#include "spot.h"

#include "types.h"
#include "callbacks.h"
#include "matrixFunctions.h"

#define TRUE 1
#define FALSE 0

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

context_t *gctx = NULL;

/* Creates a context around geomNum spotGeom's and
   imageNum spotImage's */
context_t *contextNew(unsigned int geomNum, unsigned int imageNum) {
  const char me[]="contextNew";
  context_t *ctx;
  unsigned int gi;
  int i;
  
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
  ctx->geomNum = geomNum;

  if (2 == geomNum) {
    ctx->geom[0] = spotGeomNewSphere();
    ctx->geom[1] = spotGeomNewSquare();
    SPOT_M4_SET(ctx->geom[1]->modelMatrix,
                2.0, 0.0, 0.0, 0.0,
                0.0, 2.0, 0.0, 0.0,
                0.0, 0.0, 2.0,-1.0,
                0.0, 0.0, 0.0, 1);
  }

  if (7 == geomNum) {
    ctx->geom[0] = spotGeomNewEllipsoid();
    ctx->geom[1] = spotGeomNewSphere();
    ctx->geom[2] = spotGeomNewCone();
    ctx->geom[3] = spotGeomNewSphere();
    ctx->geom[4] = spotGeomNewCone();
    ctx->geom[5] = spotGeomNewCone();
    ctx->geom[6] = spotGeomNewCone();

    for (i = 0; i < 7; i ++) {
      scaleGeom(ctx->geom[i], 0.125);
    }

    translateGeomV(ctx->geom[1], 2.5);
    translateGeomV(ctx->geom[3], -2.5);
    translateGeomU(ctx->geom[2], 2.5);
    translateGeomU(ctx->geom[4], -2.5);
    translateGeomN(ctx->geom[5], -2.5);
    translateGeomN(ctx->geom[6], 2.5);

    scaleGeom(ctx->geom[4], -1);
    scaleGeom(ctx->geom[6], -1);
    for (i=0; i < 3*3; i++) {
      ctx->geom[4]->normalMatrix[i] *= -1;
      ctx->geom[6]->normalMatrix[i] *= -1;
    }

    SPOT_V3_SET(ctx->geom[0]->objColor, 1.0f, 0.0f, 0.0f); // Red
    SPOT_V3_SET(ctx->geom[6]->objColor, 1.0f, 1.0f, 0.0f); // Yellow
    SPOT_V3_SET(ctx->geom[2]->objColor, 1.0f, 0.5f, 0.0f); // Orange
    SPOT_V3_SET(ctx->geom[3]->objColor, 0.0f, 1.0f, 0.0f); // Green
    SPOT_V3_SET(ctx->geom[4]->objColor, 0.0f, 0.0f, 1.0f); // Blue
    SPOT_V3_SET(ctx->geom[5]->objColor, 1.0f, 0.0f, 1.0f); // Purple
    SPOT_V3_SET(ctx->geom[1]->objColor, 1.0f, 1.0f, 1.0f); // White

  } else if (5 == geomNum) {
    ctx->geom[0] = spotGeomNewCone();
    ctx->geom[1] = spotGeomNewSphere();
    ctx->geom[2] = spotGeomNewCube1();
    ctx->geom[3] = spotGeomNewSoftcube();
    ctx->geom[4] = spotGeomNewCube0();
  }
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

  //Set to view mode (default)
  ctx->viewMode = 1;
  ctx->modelMode = 0;

  //Model Initializations
  SPOT_M4_IDENTITY(gctx->model.xyzw);
  SPOT_M4_IDENTITY(gctx->model.custom);

  //Camera Initializations
  SPOT_M4_IDENTITY(gctx->camera.uvn);
  SPOT_M4_IDENTITY(gctx->camera.proj);

  gctx->camera.ortho = 1;
  gctx->camera.fixed = 1;
  gctx->camera.fov = 1.57079633; // 90 degrees
  gctx->camera.near = -2;
  gctx->camera.far = 2;
  gctx->camera.up[0] = 0;
  gctx->camera.up[1] = 1;
  gctx->camera.up[2] = 0;
  gctx->camera.from[0] = 0;
  gctx->camera.from[1] = 0;
  gctx->camera.from[2] = -1;
  gctx->camera.at[0] = 0;
  gctx->camera.at[1] = 0;
  gctx->camera.at[2] = 0;

  gctx->mouseFun.m = NULL;
  gctx->mouseFun.f = identity;
  gctx->mouseFun.offset = gctx->mouseFun.multiplier = gctx->mouseFun.i = 0;

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

int contextDraw(context_t *ctx)
{
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

  norm_M4(gctx->camera.uvn);
  glUniformMatrix4fv(glGetUniformLocation(ctx->program, "viewMatrix"), 1, GL_FALSE,
      gctx->camera.uvn);
  glUniformMatrix4fv(glGetUniformLocation(ctx->program, "projMatrix"), 1, GL_FALSE,
      gctx->camera.proj);
  glUniform3fv(glGetUniformLocation(ctx->program, "lightDir"), 1, ctx->lightDir);
  //colorLoc = glGetUniformLocation(ctx->program, "objColor");

  for (gi = 0; gi < ctx->geomNum; gi++) {
    glUniform3fv(colorLoc, 1, ctx->geom[gi]->color);

    norm_M4(gctx->geom[gi]->modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(ctx->program, "modelMatrix"), 1, GL_FALSE,
        gctx->geom[gi]->modelMatrix);

    updateNormals(gctx->geom[gi]->normalMatrix, gctx->geom[gi]->modelMatrix);
    glUniformMatrix3fv(glGetUniformLocation(ctx->program, "normalMatrix"), 1, GL_FALSE, 
        gctx->geom[gi]->normalMatrix);
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
  return 0;
}

/*
** The program as given takes two command-line arguments: the vertex
** and fragment shader program filenames.  If you add more (though you
** shouldn't have to), document their usage here.
*/
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

  glfwSetWindowTitle("Project 1: Shir Yehoshua");
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
