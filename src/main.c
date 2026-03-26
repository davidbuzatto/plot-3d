/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Main function and logic for the game. Simplified template for game
 * development in C using Raylib (https://www.raylib.com/).
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "raylib/raylib.h"
#include "raylib/rlgl.h"
//#include "raylib/raymath.h"

#define X_MIN -20.0f
#define X_MAX 20.0f
#define Y_MIN -20.0f
#define Y_MAX 20.0f
#define STEPS 100
#define RADIUS 200

static Camera3D camera;
static Model surfaceModel1;
static Model surfaceModel2;
static float angle;
static float distance;

bool drawModel1 = true;
bool drawModel2 = true;

static float f1( float x, float y ) {
    //return ( x + y ) * 0.5f;
    //return x + y;
    //return sinf(x) * 2 + y;
    //return x + cosf(y) * 2;
    //return sinf(x) * 2 + cosf(y) * 2;
    //return sinf(x) * cosf(y) * 3.0f;
    //return sqrtf(x*x + y*y);
    //return x*y + y*x;
    //return tanf(x) * 2 + y;
    //return x*x + y*y;
    return sqrtf( RADIUS - x*x - y*y ) ;
}

static float f2( float x, float y ) {
    return -sqrtf( RADIUS - x*x - y*y ) ;
}

static Mesh createSurfaceModel( float xMin, float xMax, float yMin, float yMax, int steps, float (*f)( float, float ) ) {

    int cols = steps + 1;
    int rows = steps + 1;
    int vertexCount = cols * rows;
    int triangleCount = steps * steps * 2;

    Mesh mesh = { 0 };
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;
    mesh.vertices = (float *) malloc( vertexCount * 3 * sizeof( float ) );
    mesh.normals = (float *) malloc( vertexCount * 3 * sizeof( float ) );
    mesh.texcoords = (float *) malloc( vertexCount * 2 * sizeof( float ) );
    mesh.indices = (unsigned short *) malloc( triangleCount * 3 * sizeof( unsigned short ) );

    float dx = ( xMax - xMin ) / steps;
    float dy = ( yMax - yMin ) / steps;

    for ( int xi = 0; xi < cols; xi++ ) {
        for ( int yi = 0; yi < rows; yi++ ) {

            int vi = xi * rows + yi;

            float x = xMin + xi * dx;
            float y = yMin + yi * dy;
            float z = f( x, y );

            /*mesh.vertices[vi * 3 + 0] = x;
            mesh.vertices[vi * 3 + 1] = y;
            mesh.vertices[vi * 3 + 2] = z;*/

            mesh.vertices[vi * 3 + 0] = x;
            mesh.vertices[vi * 3 + 1] = z;
            mesh.vertices[vi * 3 + 2] = y;

            mesh.texcoords[vi * 2 + 0] = (float) xi / steps;
            mesh.texcoords[vi * 2 + 1] = (float) yi / steps;

            // normal
            float h = 0.001f;
            float dfdx = ( f( x + h, y ) - f( x - h, y ) ) / ( 2.0f * h );
            float dfdy = ( f( x, y + h ) - f( x, y - h ) ) / ( 2.0f * h );
            float nx = -dfdx;
            float ny = 1.0f;
            float nz = -dfdy;
            float len = sqrtf( nx * nx + ny * ny + nz * nz );
            mesh.normals[vi * 3 + 0] = nx / len;
            mesh.normals[vi * 3 + 1] = ny / len;
            mesh.normals[vi * 3 + 2] = nz / len;

        }
    }

    // índices - dois triângulos por quad
    int ii = 0;
    for ( int xi = 0; xi < steps; xi++ ) {
        for ( int yi = 0; yi < steps; yi++ ) {

            //    v0 -- v2
            //     |  \  |
            //    v1 -- v3

            unsigned short v0 = xi * rows + yi;
            unsigned short v1 = xi * rows + ( yi + 1 );
            unsigned short v2 = ( xi + 1 ) * rows + yi;
            unsigned short v3 = ( xi + 1 ) * rows + ( yi + 1 );

            mesh.indices[ii++] = v0;
            mesh.indices[ii++] = v2;
            mesh.indices[ii++] = v1;

            mesh.indices[ii++] = v1;
            mesh.indices[ii++] = v2;
            mesh.indices[ii++] = v3;

        }
    }

    UploadMesh( &mesh, false );
    return mesh;

}

static Mesh createSphereSurfaceModel( float xMin, float xMax, float yMin, float yMax, int steps, float (*f)( float, float ) ) {

    int cols = steps + 1;
    int rows = steps + 1;
    int vertexCount = cols * rows;
    int triangleCount = steps * steps * 2;

    Mesh mesh = { 0 };
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;
    mesh.vertices = (float *) malloc( vertexCount * 3 * sizeof( float ) );
    mesh.normals = (float *) malloc( vertexCount * 3 * sizeof( float ) );
    mesh.texcoords = (float *) malloc( vertexCount * 2 * sizeof( float ) );
    mesh.indices = (unsigned short *) malloc( triangleCount * 3 * sizeof( unsigned short ) );

    float dx = ( xMax - xMin ) / steps;
    float dy = ( yMax - yMin ) / steps;

    for ( int xi = 0; xi < cols; xi++ ) {
        for ( int yi = 0; yi < rows; yi++ ) {

            int vi = xi * rows + yi;

            /*float x = xMin + xi * dx;
            float y = yMin + yi * dy;
            float z = f( x, y );*/

            float x = xMin + xi * dx;
            float y = yMin + yi * dy;
            float r2 = x * x + y * y;
            float z;
            if ( r2 <= RADIUS ) {
                z = f( x, y );
            } else {
                // projeta no equador (círculo de raio sqrt(RADIUS))
                float scale = sqrtf( (float) RADIUS / r2 );
                x = x * scale;
                y = y * scale;
                z = 0.0f;
            }

            /*mesh.vertices[vi * 3 + 0] = x;
            mesh.vertices[vi * 3 + 1] = y;
            mesh.vertices[vi * 3 + 2] = z;*/

            mesh.vertices[vi * 3 + 0] = x;
            mesh.vertices[vi * 3 + 1] = z;
            mesh.vertices[vi * 3 + 2] = y;

            mesh.texcoords[vi * 2 + 0] = (float) xi / steps;
            mesh.texcoords[vi * 2 + 1] = (float) yi / steps;

            // normal
            float h = 0.001f;
            float dfdx = ( f( x + h, y ) - f( x - h, y ) ) / ( 2.0f * h );
            float dfdy = ( f( x, y + h ) - f( x, y - h ) ) / ( 2.0f * h );
            float nx = -dfdx;
            float ny = 1.0f;
            float nz = -dfdy;
            float len = sqrtf( nx * nx + ny * ny + nz * nz );
            mesh.normals[vi * 3 + 0] = nx / len;
            mesh.normals[vi * 3 + 1] = ny / len;
            mesh.normals[vi * 3 + 2] = nz / len;

        }
    }

    // índices - dois triângulos por quad
    int ii = 0;
    for ( int xi = 0; xi < steps; xi++ ) {
        for ( int yi = 0; yi < steps; yi++ ) {

            //    v0 -- v2
            //     |  \  |
            //    v1 -- v3

            unsigned short v0 = xi * rows + yi;
            unsigned short v1 = xi * rows + ( yi + 1 );
            unsigned short v2 = ( xi + 1 ) * rows + yi;
            unsigned short v3 = ( xi + 1 ) * rows + ( yi + 1 );

            mesh.indices[ii++] = v0;
            mesh.indices[ii++] = v2;
            mesh.indices[ii++] = v1;

            mesh.indices[ii++] = v1;
            mesh.indices[ii++] = v2;
            mesh.indices[ii++] = v3;

        }
    }

    UploadMesh( &mesh, false );
    return mesh;

}

static void init( void ) {

    camera = (Camera3D) {
        .position = { 0.0f, 10.0f, 0.0f },
        .target = { 0.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fovy = 60.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    
    //Mesh mesh1 = createSurfaceModel( X_MIN, X_MAX, Y_MIN, Y_MAX, STEPS, f1 );

    Mesh mesh1 = createSphereSurfaceModel( X_MIN, X_MAX, Y_MIN, Y_MAX, STEPS, f1 );
    surfaceModel1 = LoadModelFromMesh( mesh1 );
    surfaceModel1.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

    Mesh mesh2 = createSphereSurfaceModel( X_MIN, X_MAX, Y_MIN, Y_MAX, STEPS, f2 );
    surfaceModel2 = LoadModelFromMesh( mesh2 );
    surfaceModel2.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

    angle = 0.0f;
    distance = 50.0f;

}

static void update( float delta ) {

    if ( IsKeyPressed( KEY_ONE ) ) {
        drawModel1 = !drawModel1;
    }

    if ( IsKeyPressed( KEY_TWO ) ) {
        drawModel2 = !drawModel2;
    }

    if ( IsKeyDown( KEY_LEFT ) ) { 
        angle--;
    }

    if ( IsKeyDown( KEY_RIGHT ) ) { 
        angle++;
    }

    if ( IsKeyDown( KEY_UP ) ) { 
        camera.position.y++; 
        //camera.target.y++;
    }

    if ( IsKeyDown( KEY_DOWN ) ) {
        camera.position.y--;
        //camera.target.y--;
    }

    if ( IsKeyDown( KEY_A ) ) {
        distance++;
    }
    
    if ( IsKeyDown( KEY_S ) ) { 
        distance--;
    }

    if ( distance <= 1.0f ) {
        distance = 1.0f;
    }

    camera.position.x = sinf( DEG2RAD * angle ) * distance;
    camera.position.z = cosf( DEG2RAD * angle ) * distance;

}

static void draw( void ) {

    BeginDrawing();
    ClearBackground( WHITE );

    BeginMode3D( camera );
    //DrawGrid( 100, 1.0f );

    // references
    /*DrawSphere( (Vector3) { -10, 0, -10 }, 0.5f, BLUE );
    DrawSphere( (Vector3) { -10, 0, 10 }, 0.5f, RED );
    DrawSphere( (Vector3) { 10, 0, -10 }, 0.5f, GREEN );
    DrawSphere( (Vector3) { 10, 0, 10 }, 0.5f, YELLOW );*/
    //DrawSphereWires( (Vector3) { 10, 0, 10 }, 0.55f, 10, 10, BLACK );

    rlDisableBackfaceCulling();

    if ( drawModel1 ) {
        DrawModel( surfaceModel1, (Vector3) {0}, 1.0f, WHITE );
        DrawModelWires( surfaceModel1, (Vector3) {0}, 1.0f, BLACK );
    }

    if ( drawModel2 ) {
        DrawModel( surfaceModel2, (Vector3) {0}, 1.0f, WHITE );
        DrawModelWires( surfaceModel2, (Vector3) {0}, 1.0f, BLACK );
    }

    rlEnableBackfaceCulling();

    /*for ( float x = X_MIN; x <= X_MAX; x++ ) {
        for ( float y = Y_MIN; y <= Y_MAX; y++ ) {
            DrawSphere( (Vector3) { x, y, f( x, y ) }, 0.5f, BLACK );
        }
    }*/

    EndMode3D();
    EndDrawing();

}

int main( void ) {
    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( 800, 800, "Plot 3D" );
    SetTargetFPS( 60 );
    init();
    while ( !WindowShouldClose() ) {
        update( GetFrameTime() );
        draw();
    }
    CloseWindow();
    return 0;
}