
#include <GLUT/glut.h>
#include "glsupport.h"
#include "matrix4.h"

GLuint program;

GLuint modelviewMatrixUniformLocation;
GLuint projectionMatrixUniformLocation;
GLuint normalMatrixUniformLocation;
GLuint uColorUniformLocation;

//GLuint colorAttribute;
GLuint positionAttribute;
GLuint normalAttribute;

GLuint cubeBuffer;
//GLuint colorBuffer;
GLuint normalBuffer;

Matrix4 eyeMatrix;
class Entity {
public:
    Cvec3 translation;
    Cvec3 rotation;
    Cvec3 scale;
    
    Matrix4 modelMatrix;
    Entity *parent;
    
    Entity()
    {
        translation = Cvec3(0,0,0);
        rotation = Cvec3(0,0,0);
        scale = Cvec3(1,1,1);
        parent = NULL;
    }
    
    void Render()
    {
        GeometricOperations();
        Matrix4 modelViewMatrix = inv(eyeMatrix) * this->modelMatrix;
        GLfloat glmatrix[16];
        modelViewMatrix.writeToColumnMajorMatrix(glmatrix);
        
        //Give the vertex shader the ModelViewMatrix
        glUniformMatrix4fv(modelviewMatrixUniformLocation, 1, false, glmatrix);
        
        Matrix4 normalMatrix = transpose(inv(modelViewMatrix));
        GLfloat glNormalMatrix[16];
        normalMatrix.writeToColumnMajorMatrix(glNormalMatrix);
        glUniformMatrix4fv(normalMatrixUniformLocation, 1, false, glNormalMatrix);
        
        Matrix4 projectionMatrix;
        projectionMatrix = projectionMatrix.makeProjection(45.0, 1.0, -0.1, -200.0);
        GLfloat glmatrixProjection[16];
        projectionMatrix.writeToColumnMajorMatrix(glmatrixProjection);
        
        //Give the vertex shader the projectionMatrix
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, false, glmatrixProjection);
        
    }
    
    void GeometricOperations()
    {
        if(parent != NULL)
        {
            modelMatrix = parent->modelMatrix * modelMatrix;
        }
        modelMatrix = modelMatrix * modelMatrix.makeTranslation(translation);
        modelMatrix = modelMatrix * modelMatrix.makeXRotation((double)rotation[0]) * modelMatrix.makeYRotation((double)rotation[1]) * modelMatrix.makeZRotation((double)rotation[2]);
        modelMatrix = modelMatrix * modelMatrix.makeScale(scale);
    }
    
};

void display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    Entity base;
    Entity base2;
    Entity base3;
    
    glUniform3f(uColorUniformLocation, 0.1f, 0.9f, 0.3f);
    
    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    base.rotation = {0,(float)timeSinceStart/30.0f , 0};
    base.scale = Cvec3(2.0,2.0,2.0);
    base.Render();
    
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttribute);
    /*
     glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
     glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
     glEnableVertexAttribArray(colorAttribute);
     */
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    base2.translation = Cvec3(5.0,0.0,0.0);
    base2.rotation = Cvec3(((float)timeSinceStart/30.0f),0, 0);
    base2.parent = &base;
    base2.Render();
    glUniform3f(uColorUniformLocation, 0.9f, 0.9f, 0.3f);

    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    base3.translation = Cvec3(2.0,2.0,2.0);
    base3.rotation = Cvec3(0,0,(float)timeSinceStart/30.0f);
    base3.scale = Cvec3(0.4,0.4,0.4);
    base3.parent = &base2;
    base3.Render();
    glUniform3f(uColorUniformLocation, 0.0f, 0.5f, 0.9f);

    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    //glDisableVertexAttribArray(colorAttribute);
    glDisableVertexAttribArray(normalAttribute);
    glDisableVertexAttribArray(positionAttribute);
    
    glutSwapBuffers();
    
}

void init() {
    
    glClearDepth(0.0f);
    program = glCreateProgram();
    readAndCompileShader(program, "vertex.glsl", "fragment.glsl");
    
    glUseProgram(program);
    
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glReadBuffer(GL_BACK);
    
    eyeMatrix = eyeMatrix.makeTranslation(Cvec3(0.0, 0.0, 60.0));
    
    modelviewMatrixUniformLocation = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixUniformLocation = glGetUniformLocation(program, "projectionMatrix");
    normalMatrixUniformLocation = glGetUniformLocation(program, "normalMatrix");
    uColorUniformLocation = glGetUniformLocation(program, "uColor");
    
    // colorAttribute = glGetAttribLocation(program, "color");
    positionAttribute = glGetAttribLocation(program, "position");
    normalAttribute = glGetAttribLocation(program, "normal");
    
    //Position Buffer Data
    //Process --> Generate buffer, Bind the buffer and then input the bufferdata into the binded buffer.
    
    glGenBuffers(1, &cubeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
    
    GLfloat cubeVerts[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    
    glBufferData(GL_ARRAY_BUFFER, 108*sizeof(GLfloat), cubeVerts, GL_STATIC_DRAW);
    /*
     glGenBuffers(1, &colorBuffer);
     glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
     
     GLfloat cubeColors[] = {
     0.583f,  0.771f,  0.014f, 1.0f,
     0.609f,  0.115f,  0.436f, 1.0f,
     0.327f,  0.483f,  0.844f, 1.0f,
     0.822f,  0.569f,  0.201f, 1.0f,
     0.435f,  0.602f,  0.223f, 1.0f,
     0.310f,  0.747f,  0.185f, 1.0f,
     0.597f,  0.770f,  0.761f, 1.0f,
     0.559f,  0.436f,  0.730f, 1.0f,
     0.359f,  0.583f,  0.152f, 1.0f,
     0.483f,  0.596f,  0.789f, 1.0f,
     0.559f,  0.861f,  0.639f, 1.0f,
     0.195f,  0.548f,  0.859f, 1.0f,
     0.014f,  0.184f,  0.576f, 1.0f,
     0.771f,  0.328f,  0.970f, 1.0f,
     0.406f,  0.615f,  0.116f, 1.0f,
     0.676f,  0.977f,  0.133f, 1.0f,
     0.971f,  0.572f,  0.833f, 1.0f,
     0.140f,  0.616f,  0.489f, 1.0f,
     0.997f,  0.513f,  0.064f, 1.0f,
     0.945f,  0.719f,  0.592f, 1.0f,
     0.543f,  0.021f,  0.978f, 1.0f,
     0.279f,  0.317f,  0.505f, 1.0f,
     0.167f,  0.620f,  0.077f, 1.0f,
     0.347f,  0.857f,  0.137f, 1.0f,
     0.055f,  0.953f,  0.042f, 1.0f,
     0.714f,  0.505f,  0.345f, 1.0f,
     0.783f,  0.290f,  0.734f, 1.0f,
     0.722f,  0.645f,  0.174f, 1.0f,
     0.302f,  0.455f,  0.848f, 1.0f,
     0.225f,  0.587f,  0.040f, 1.0f,
     0.517f,  0.713f,  0.338f, 1.0f,
     0.053f,  0.959f,  0.120f, 1.0f,
     0.393f,  0.621f,  0.362f, 1.0f,
     0.673f,  0.211f,  0.457f, 1.0f,
     0.820f,  0.883f,  0.371f, 1.0f,
     0.982f,  0.099f,  0.879f, 1.0f
     };
     
     glBufferData(GL_ARRAY_BUFFER, 144*sizeof(GLfloat), cubeColors, GL_STATIC_DRAW);
     */
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    
    GLfloat cubeNormals[] = {
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f,-1.0f,0.0f,
        0.0f,-1.0f,0.0f,
        0.0f,-1.0f,0.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        -1.0f, 0.0f,0.0f,
        -1.0f, 0.0f,0.0f,
        -1.0f, 0.0f,0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f,0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f,0.0f, 1.0f,
        0.0f,0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f,0.0f,0.0f,
        1.0f, 0.0f,0.0f,
        1.0f,0.0f,0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f,0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f,0.0f,
        0.0f, 1.0f,0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f,0.0f, 1.0f
    };
    
    glBufferData(GL_ARRAY_BUFFER, 108*sizeof(GLfloat), cubeNormals, GL_STATIC_DRAW);
    
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Project-N11028457");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
    init();
    glutMainLoop();
    return 0;
}


