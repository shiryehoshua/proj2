#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#else
#  include <GL/gl3.h>
#endif
#include "spot.h"

#include "types.h"
#include "callbacks.h"
#include "matrixFunctions.h"

extern context_t *gctx;
extern void setScene(int i);
extern int contextDraw(context_t *ctx);


#define UP 283
#define DOWN 284
#define LEFT 285
#define RIGHT 286
void callbackKeyboard(int key, int action)
{
  if (GLFW_PRESS != action) {
    GLfloat v;
    switch (key) {
      // Quit the application
      case 'Q': gctx->running=0; break;

      case 'U': gctx->camera.fixed ^= 1;
        fprintf(stderr, gctx->camera.fixed ? "Up vector fixed\n" : "Up vector free\n");
        break;

      // Toggle between orthographic and perspective projection matrices
      case 'P':
        gctx->camera.ortho ^= 1;
        fprintf(stderr, gctx->camera.ortho ? "Orthographic\n" : "Perspective\n");
        callbackResize(gctx->winSizeX, gctx->winSizeY);
        break;

      // Enter View Mode
      case 'V':
        gctx->viewMode = 1;
        gctx->modelMode = gctx->lightMode = 0;
        fprintf(stderr,"View Mode\n");
        break;

      // Enter Model Mode
      case 'M':
        gctx->modelMode = 1;
        gctx->viewMode = gctx->lightMode = 0;
        fprintf(stderr,"Model Mode\n");
        break;

      // Enter Light Mode
      case 'L':
        gctx->lightMode = 1;
        gctx->viewMode = gctx->modelMode = 0;
        fprintf(stderr,"Light Mode\n");
        break;

      // Rotate around the U-axis of the viewport
      case UP:
        v = 0.125;
        rotate_view_U(v);
        break;

      // Rotate around the U-axis of the viewport
      case DOWN:
        v= -0.125;
        rotate_view_U(v);
        break;

      // Rotate around the V-axis of the viewport
      case LEFT:
        v= -0.125;
        rotate_view_V(v);
        break;

      // Rotate around the V-axis of the viewport
      case RIGHT:
        v = 0.125;
        rotate_view_V(v);
        break;

      // Describe and display scene 1
      case '1':
        fprintf(stderr, "Setting scene 1: Demonstrating model, view and orthographic view transoforms\n");
        setScene(1);
        break;

      // Describe and display scene 2
      case '2':
        fprintf(stderr, "Setting scene 2: Demonstrating perspective transform\n");
        setScene(2);
        break;

      // Describe and display scene 3
      case '3':
        fprintf(stderr, "Setting scene 3: Demostrating correct surface normals\n"); 
        setScene(3);
        break;

      // Print keycode for debugging purposes
      default:
        fprintf(stderr, "Caught key code: %d\n", key);
    }
  }
}

#define FIFTH 0.2
#define VERTICAL 1
#define HORIZONTAL 0
void callbackMouseButton(int button, int action)
{
  (void)(button);
  int xx, yy;
  float xf, yf;
  gctx->buttonDown = 1;
  gctx->shiftDown = GLFW_PRESS == glfwGetKey(GLFW_KEY_LSHIFT)
                 || GLFW_PRESS == glfwGetKey(GLFW_KEY_RSHIFT);
  glfwGetMousePos(&xx, &yy);
  gctx->lastX = xx;
  gctx->lastY = yy;

  gctx->mouseFun.m = gctx->geom[0]->modelMatrix; // To be overwritten
  gctx->mouseFun.f = identity;
  gctx->mouseFun.offset = 0;
  gctx->mouseFun.multiplier = 1;

  if (GLFW_PRESS == action) {
    xf = (float) xx / gctx->winSizeX;
    yf = (float) yy / gctx->winSizeY;
    if (yf > 1 - FIFTH) {
      gctx->mouseFun.i = HORIZONTAL;
      if (!gctx->shiftDown) {
        if (gctx->viewMode) {
          printf(" ... (mode V) rotates eye point around N\n");
          gctx->mouseFun.m = NULL;
          gctx->mouseFun.f = m_rotate_view_N;
        } else if (gctx->lightMode) {
          printf(" ... (mode L) rotates light direction around N\n");
          gctx->mouseFun.m = gctx->lightDir;
          gctx->mouseFun.f = m_rotate_3rd_V3;
          gctx->mouseFun.multiplier = 10;
        }
      } else {
        if (gctx->modelMode) {
          printf(" ... (move M) translates object along N\n");
          gctx->mouseFun.m = gctx->geom[0]->modelMatrix;
          gctx->mouseFun.f = translate_model_N;
          gctx->mouseFun.multiplier = 4;
          updateNormals(gctx->geom[0]->normalMatrix, gctx->geom[0]->modelMatrix);
        } else if (gctx->viewMode) {
          printf(" ... (move V) translates eye and look-at along N\n");
          gctx->mouseFun.m = gctx->camera.uvn; // Never accessed
          gctx->mouseFun.f = translate_view_N;
          gctx->mouseFun.multiplier = -1;
        }
      }
    } else if (xf < FIFTH) {
      gctx->mouseFun.i = VERTICAL;
      if (!gctx->shiftDown) {
        printf(" ... (mode V) zooms in or out\n");
        if (gctx->viewMode) {
          gctx->mouseFun.m = &gctx->camera.fov;
          gctx->mouseFun.f = scale_1D;
          gctx->mouseFun.offset = 1;
          gctx->mouseFun.multiplier = 0.25;
        }
      } else {
        printf(" ... (mode V) shrinks or grows (far distance) - (near distance)\n");
        if (gctx->viewMode) {
          gctx->mouseFun.m = NULL;
          gctx->mouseFun.f = scale_near_far;
          gctx->mouseFun.multiplier = 0.25;
        }
      }
    } else {
      if (!gctx->shiftDown) {
        if (gctx->viewMode) {
          printf(" ... (mode V) rotates eye point around U and V\n");
          gctx->mouseFun.m = NULL;
          gctx->mouseFun.f = m_rotate_view_UV;
          gctx->mouseFun.multiplier = 2;
        } else if (gctx->lightMode) {
          printf(" ... (mode L) rotates light direction around U and V\n");
          gctx->mouseFun.m = gctx->lightDir;
          gctx->mouseFun.f = m_rotate_1st_2nd_V3;
        }
      } else {
        printf(" ... (mode V) translates eye and look-at along U and V\n");
        printf(" ... (mode M) translates object along U and V\n");
        if (gctx->viewMode) {
          gctx->mouseFun.m = gctx->camera.uvn;
          gctx->mouseFun.f = translate_view_UV;
          gctx->mouseFun.multiplier = -4;
        } else if (gctx->modelMode) {
          gctx->mouseFun.m = gctx->geom[0]->modelMatrix;
          gctx->mouseFun.f = translate_model_UV;
          gctx->mouseFun.multiplier = 4;
        }
      }
    }
  } else {
    gctx->buttonDown = gctx->shiftDown = 0;
  }
}

void callbackMousePos(int xx, int yy)
{
  GLfloat s[5];
  if (gctx->buttonDown) {
    GLfloat xf = (float) xx / gctx->winSizeX;
    GLfloat yf = (float) yy / gctx->winSizeY;
    s[0] = ((float) gctx->lastX / gctx->winSizeX) - xf;
    s[1] = ((float) gctx->lastY / gctx->winSizeY) - yf;
    s[0] = gctx->mouseFun.multiplier * s[0] + gctx->mouseFun.offset;
    s[1] = gctx->mouseFun.multiplier * s[1] + gctx->mouseFun.offset;

    (gctx->mouseFun.f)(gctx->mouseFun.m, s, gctx->mouseFun.i);

    // NOTE: We update lastX and lastY in both callbackMouseButton and callbackMousePos; We believe
    //       this produces better motion.
    gctx->lastX = xx;
    gctx->lastY = yy;
  }
}

void callbackResize(int w, int h)
{
  const char me[]="callbackResize";

  // Recalculated w and h values (using camera aspect ratio and fov); for projection matrix
  GLfloat wf, hf; 

  // NOTE: glfwSetWindowSizeCallback sometimes sends negative numbers, causing the program to
  //       crash. To counteract this, clamp w and h above 0.
  // NOTE: Still doesn't fix it... seems to bug out only when glfwSwapBuffers gets called...
  if (w<=0) w = 1;
  if (h<=0) h = 1;

  /* Set Viewport to window dimensions */
  glViewport(0, 0, w, h);

  gctx->winSizeX = w;
  gctx->winSizeY = h;

  // Update aspect ratio
  gctx->camera.aspect = (GLfloat) w / h;

  // Calculate hf and wf
  if (!gctx->camera.ortho) {
    hf = 0.5 * gctx->camera.near * tanf(0.5 * gctx->camera.fov);
    wf = gctx->camera.aspect * hf;
  } else {
    wf = gctx->camera.aspect;
    hf = 1;
  }

  // Normalize
  //wf /= hf;
  //hf = 1
  updateProj(gctx->camera.proj, wf, hf, gctx->camera.near, gctx->camera.far, gctx->camera.ortho);

  /* redraw now with new window size to permit feedback during resizing */
  if (contextDraw(gctx)) {
    spotErrorAdd("%s: trouble drawing", me);
    spotErrorPrint();
    spotErrorClear();
    gctx->running = 0;
  }
  glfwSwapBuffers();
  return;
}

void setScene(int sceneNum)
{
  int i;

  if (sceneNum == 1) {

    for (i = 0; i < gctx->geomNum; i ++) {
      // set each object to be the same size
      SPOT_M4_IDENTITY(gctx->geom[i]->modelMatrix);
      scaleGeom(gctx->geom[i], 0.125);
    }

    // set objects so that there is one on each axis
                                         // Red: is in the center
    translateGeomN(gctx->geom[6],  2.5); // Yellow: placing in the back
    translateGeomU(gctx->geom[2],  2.5); // Orange: placing on the right
    translateGeomV(gctx->geom[3], -2.5); // Green: placing on the bottom
    translateGeomU(gctx->geom[4], -2.5); // Blue: placing on left
    translateGeomN(gctx->geom[5], -2.5); // Purple: placing in the front 
    translateGeomV(gctx->geom[1],  2.5); // White: placing on top

    // set to orthographic mode
    gctx->camera.ortho = 1;
    callbackResize(gctx->winSizeX, gctx->winSizeY);

    // move from so that overlooks objects
    gctx->camera.from[0] = 1;
    gctx->camera.from[1] = 0.5;
    gctx->camera.from[2] = -1;

    fprintf(stderr, 
      "Based on placement, we know that the white sphere belongs on top,the blue cone on the left, orange cone on the right, yellow in the back, purple in the front and the green sphere on the bottom. Knowing that our model is centered at (0,0,0) it makes sense to take a look at the space from (1, 0.5, 1), that we are in orthographic mode, and that our from point is at (0, 1, -1) we know that our model view and orthographic transform are working properly.\n");

  } else if (sceneNum == 2) {

    for (i = 0; i < gctx->geomNum; i ++) {
      // set each object to be the same size
      SPOT_M4_IDENTITY(gctx->geom[i]->modelMatrix);
      scaleGeom(gctx->geom[i], 0.125);

      // set each object farther and farther away
      translateGeomN(gctx->geom[i], i*5);
    }

    // set to perspective mode
    gctx->camera.ortho = 0;
    callbackResize(gctx->winSizeX, gctx->winSizeY);

    // move from so that puts objects along a path
    gctx->camera.from[0] = 0.75;
    gctx->camera.from[1] = 0.75;
    gctx->camera.from[2] = -2;

    fprintf(stderr,
      "To ensure that prospective transform is correct, the best way to look at objects right behind each other. Thus the arrangement here is all of our objects in a row, one after another, looking at them almost head on. Note that each object is the same size, yet in the picture the objects get smaller as they are placed farther back.");
  } else if (sceneNum == 3) {

    for (i = 0; i < gctx->geomNum; i ++) {
      SPOT_M4_IDENTITY(gctx->geom[i]->modelMatrix);

      // set each object except for the sphere and ellipsoid to dissapear 
      if (i != 0 && i != 1) {
        scaleGeom(gctx->geom[i], 0);
      } else {
        scaleGeom(gctx->geom[i], 0.125);
      }
    }

    translateGeomV(gctx->geom[1], 1);

    // scale the sphere by 1.0 along the X, 0.5 along the Y, 0.2 along the Z
    scaleGeomX(gctx->geom[1], 1);
    scaleGeomY(gctx->geom[1], 0.5);
    scaleGeomZ(gctx->geom[1], 0.2);

    // set both to the same color
    SPOT_V3_SET(gctx->geom[0]->color, 1.0f, 0.0f, 0.0f); // Red
    SPOT_V3_SET(gctx->geom[1]->color, 1.0f, 0.0f, 0.0f); // Red

    // set to orthographic mode
    gctx->camera.ortho = 1;
    callbackResize(gctx->winSizeX, gctx->winSizeY);

    // set from so that overlooks objects
    gctx->camera.from[0] = 0;
    gctx->camera.from[1] = -1;
    gctx->camera.from[2] = -1;

    fprintf(stderr,
      "To ensure normals were calculated correctly, an ellipsoid whose normals are correct was placed beside a sphere that was stretched to be the same size as the sllipsoid. They were placed side by side.\n");

  } else {
    fprintf(stderr, "Cannot set up scene %d because scene %d does not exist!\n", sceneNum, sceneNum);
  }

}
