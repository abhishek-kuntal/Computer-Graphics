
#include <GLUT/glut.h>
#include "glsupport.h"
#include "matrix4.h"
#include "quat.h"
#include "cvec.h"

#include "geometrymaker.h"
#include "stb_image.h"
 #define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

GLuint program;

GLuint modelviewMatrixUniformLocation;
GLuint projectionMatrixUniformLocation;
GLuint normalMatrixUniformLocation;
GLuint uColorUniformLocation;

GLuint positionAttribute;
GLuint normalAttribute;
GLuint texCoordAttribute;
GLuint binormalAttribute;
GLuint tangentAttribute;

GLuint lightColorUniformLocation;
GLuint lightLightColorUniformLocation;
GLuint lightSpecularColorUniformLocation;

GLuint lightPositioUniformLocation0;
GLuint lightPositioUniformLocation1;
GLuint lightColorUniformLocation0;
GLuint lightColorUniformLocation1;
GLuint lightSpecularColorUniformLocation0;
GLuint lightSpecularColorUniformLocation1;

GLuint diffuseTexture;
GLuint specularTexture;
GLuint normalTexture;

GLuint diffuseTexture1;
GLuint specularTexture1;
GLuint normalTexture1;

GLuint useSpecular; //Using it for multiple models showcase for the assignment

GLuint diffuseTextureUniformLocation;
GLuint specularUniformLocation;
GLuint normalUniformLocation;

bool alreadyLoaded; //Check to not load the model again and again if using geometry maker or loadobj in display

GLuint vertexBO;
GLuint normalBO;

Matrix4 eyeMatrix;

struct VertexData {
    Cvec3f p, n, b, tg;
    Cvec2f t;
    VertexData() {}
    VertexData(float x, float y, float z, float nx, float ny, float nz) : p(x,y,z), n(nx, ny, nz) {}
    VertexData& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        t = v.tex;
        b = v.binormal;
        tg = v.tangent;
        return *this;
    } };


std::vector<VertexData> meshVertices;
std::vector<unsigned short> meshIndices;
std::vector<VertexData> planeVertex;
std::vector<unsigned short> planeIndex;

struct Entity {
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
    
    void Draw(GLuint size, bool isModel) {
        
        GeometricOperations();
        
        Matrix4 modelViewMatrix = inv(eyeMatrix) * this->modelMatrix;
        GLfloat glmatrix[16];
        modelViewMatrix.writeToColumnMajorMatrix(glmatrix);
        
        //Give the vertex shader the ModelViewMatrix
        glUniformMatrix4fv(modelviewMatrixUniformLocation, 1, false, glmatrix);
        
        Matrix4 invm = inv(modelViewMatrix);
        invm(0, 3) = invm(1, 3) = invm(2, 3) = 0;
        Matrix4 normalMatrix = transpose(invm);
        
        GLfloat glNormalMatrix[16];
        normalMatrix.writeToColumnMajorMatrix(glNormalMatrix);
        glUniformMatrix4fv(normalMatrixUniformLocation, 1, false, glNormalMatrix);
        
        Matrix4 projectionMatrix;
        projectionMatrix = projectionMatrix.makeProjection(45.0, 1.0, -0.1, -200.0);
        GLfloat glmatrixProjection[16];
        projectionMatrix.writeToColumnMajorMatrix(glmatrixProjection);
        
        //Give the vertex shader the projectionMatrix
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, false, glmatrixProjection);
        //geometry.Draw(posAttribute, norAttribute);
        
        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, p));
        glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
        
        glEnableVertexAttribArray(normalAttribute);
        glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, n));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBO);
        
        glEnableVertexAttribArray(texCoordAttribute);
        glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, t));
        
        glEnableVertexAttribArray(binormalAttribute);
        glVertexAttribPointer(binormalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, b));
        
        glEnableVertexAttribArray(tangentAttribute);
        glVertexAttribPointer(tangentAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tg));
        
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
        
        glDisableVertexAttribArray(positionAttribute);
        glDisableVertexAttribArray(normalAttribute);
        glDisableVertexAttribArray(texCoordAttribute);
        glDisableVertexAttribArray(binormalAttribute);
        glDisableVertexAttribArray(tangentAttribute);
    
    }
    
    void GeometricOperations()
    {
        if(parent != NULL)
        {
            modelMatrix = parent->modelMatrix * modelMatrix;
        }
        modelMatrix = modelMatrix * modelMatrix.makeTranslation(translation);
        Quat q1 = Quat::makeXRotation((double)rotation[0]);
        Quat q2 = Quat::makeYRotation((double)rotation[1]);
        Quat q3 = Quat::makeZRotation((double)rotation[2]);
        Quat combined = q1 * q2 * q3;
        Matrix4 rotationMatrix = quatToMatrix(combined);
        modelMatrix = modelMatrix * rotationMatrix;
        modelMatrix = modelMatrix * modelMatrix.makeScale(scale);
    }
    
};

void calculateFaceTangent(const Cvec3f &v1, const Cvec3f &v2, const Cvec3f &v3, const Cvec2f &texCoord1, const Cvec2f &texCoord2,
                          const Cvec2f &texCoord3, Cvec3f &tangent, Cvec3f &binormal) {
    Cvec3f side0 = v1 - v2;
    Cvec3f side1 = v3 - v1;
    Cvec3f normal = cross(side1, side0);
    normalize(normal);
    float deltaV0 = texCoord1[1] - texCoord2[1];
    float deltaV1 = texCoord3[1] - texCoord1[1];
    tangent = side0 * deltaV1 - side1 * deltaV0;
    normalize(tangent);
    float deltaU0 = texCoord1[0] - texCoord2[0];
    float deltaU1 = texCoord3[0] - texCoord1[0];
    binormal = side0 * deltaU1 - side1 * deltaU0;
    normalize(binormal);
    Cvec3f tangentCross = cross(tangent, binormal);
    if (dot(tangentCross, normal) < 0.0f) {
        tangent = tangent * -1;
    } }


void PrepareBuffer(std::vector<VertexData> vtx, std::vector<unsigned short> idx, bool isModel = false)
{
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
    
    if(isModel)
    {
        glUniform1i(diffuseTextureUniformLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
       
        glUniform1i(specularUniformLocation, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTexture);
        
        glUniform1i(normalUniformLocation, 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
    }
    else
    {
        glUniform1i(diffuseTextureUniformLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture1);
        
        glUniform1i(specularUniformLocation, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTexture1);
        
        glUniform1i(normalUniformLocation, 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalTexture1);

    }
    
}

void loadObjFile(const std::string &fileName, std::vector<VertexData> &outVertices, std::vector<unsigned short> &outIndices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), NULL, true);
    if(ret) {
        for(int i=0; i < shapes.size(); i++) {
            for(int j=0; j < shapes[i].mesh.indices.size(); j++) {
                unsigned int vertexOffset = shapes[i].mesh.indices[j].vertex_index * 3;
                unsigned int normalOffset = shapes[i].mesh.indices[j].normal_index * 3;
                unsigned int texOffset = shapes[i].mesh.indices[j].texcoord_index * 2;
                VertexData v;
                v.p[0] = attrib.vertices[vertexOffset];
                v.p[1] = attrib.vertices[vertexOffset+1];
                v.p[2] = attrib.vertices[vertexOffset+2];
                v.n[0] = attrib.normals[normalOffset];
                v.n[1] = attrib.normals[normalOffset+1];
                v.n[2] = attrib.normals[normalOffset+2];
                v.t[0] = attrib.texcoords[texOffset];
                v.t[1] = 1.0-attrib.texcoords[texOffset+1];
                outVertices.push_back(v);
                outIndices.push_back(outVertices.size()-1);
            } }
        
        for(int i=0; i < outVertices.size(); i += 3) {
            Cvec3f tangent;
            Cvec3f binormal;
            calculateFaceTangent(outVertices[i].p, outVertices[i+1].p, outVertices[i+2].p,
                                 outVertices[i].t, outVertices[i+1].t, outVertices[i+2].t, tangent, binormal);
            outVertices[i].tg = tangent;
            outVertices[i+1].tg = tangent;
            outVertices[i+2].tg = tangent;
            outVertices[i].b = binormal;
            outVertices[i+1].b = binormal;
            outVertices[i+2].b = binormal;
            //std::cout<<tangent[0]<<" "<<tangent[1]<<endl;
        }
    } else {
        std::cout << err << std::endl;
        assert(false);
    }
}


void loadPlane()
{
    int vb,ib;
    getPlaneVbIbLen(vb, ib);
    planeIndex.resize(ib);
    planeVertex.resize(vb);
    makePlane(10, planeVertex.begin(), planeIndex.begin());
}


void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    float timeSinceStart = sin(glutGet(GLUT_ELAPSED_TIME)/1000.0f);
    Matrix4 invm1 = inv(eyeMatrix);
    
    //Moving Light
    Cvec4 lightpos = Cvec4(timeSinceStart*20.0f, 5.0f,-40.0f, 0.0);
    lightpos = invm1 * lightpos;
    glUniform3f(lightPositioUniformLocation0, lightpos[0], lightpos[1], lightpos[2]);
    glUniform3f(lightColorUniformLocation0, 0.6f, 0.6f, 0.6f);
    glUniform3f(lightSpecularColorUniformLocation0, 1.0f, 0.0f, 0.0f);
    
    //Fixed Light
    lightpos = Cvec4(0.0, 0.0f, -40.0f, 0.0);
    lightpos = invm1 * lightpos;
    glUniform3f(lightPositioUniformLocation1, lightpos[0], lightpos[1], lightpos[2]);
    glUniform3f(lightColorUniformLocation1, 0.6f,0.6f,0.6f);
    glUniform3f(lightSpecularColorUniformLocation1, 1.0f, 0.5f,0.0f);
    
    Entity baseeFloor;
    Entity baseNoTextureNoNormalNoSpecular;
    Entity baseNoTextureNoNormal;
    Entity baseNoTextureNormal;
    Entity baseTextureNoNormalNoSpecular;
    Entity baseTextureNoNormal;
    Entity baseTextureNormal;
    
    PrepareBuffer(planeVertex, planeIndex, false);
    
    glUniform3f(useSpecular, 0.0f,0.0f,0.0f);
    baseeFloor.rotation = Cvec3(20.0,0.0,0.0);
    baseeFloor.translation = Cvec3(0.0,-10.0,0.0);
    baseeFloor.scale = Cvec3(10.0,10.0,10.0);
    baseeFloor.Draw((int)planeIndex.size(), true);
  
    PrepareBuffer(meshVertices, meshIndices, true);

    glUniform3f(useSpecular, 1.0f,1.0f,0.0f);
    baseNoTextureNoNormalNoSpecular.translation = Cvec3(-15,-5,0);
    baseNoTextureNoNormalNoSpecular.Draw((int)meshIndices.size(),true);
   
    glUniform3f(useSpecular, 2.0f,1.0f,1.0f);
    baseNoTextureNoNormal.translation = Cvec3(-9,-5,0);
    baseNoTextureNoNormal.Draw((int)meshIndices.size(),true);
   
    glUniform3f(useSpecular, 1.0f,0.0f,0.0f);
    baseNoTextureNormal.translation = Cvec3(-3,-5,0);
    baseNoTextureNormal.Draw((int)meshIndices.size(),true);
    
    glUniform3f(useSpecular, 3.0f,1.0f,2.0f);
    baseTextureNoNormalNoSpecular.translation = Cvec3(3,-5,0);
    baseTextureNoNormalNoSpecular.Draw((int)meshIndices.size(),true);
    
    glUniform3f(useSpecular, 0.0f,1.0f,0.0f);
    baseTextureNoNormal.translation = Cvec3(9,-5,0);
    baseTextureNoNormal.Draw((int)meshIndices.size(),true);
    
    glUniform3f(useSpecular, 0.0f,0.0f,0.0f);
    baseTextureNormal.translation = Cvec3(15,-5,0);
    
    baseTextureNormal.Draw((int)meshIndices.size(),true);
    
    alreadyLoaded = true;
    
    glutSwapBuffers();

}

void loadShadersObjects()
{
    //Load Objects and shaders
    loadObjFile("Monk_Giveaway_Fixed.obj", meshVertices, meshIndices);
    diffuseTexture = loadGLTexture("Monk_D.tga");
    specularTexture = loadGLTexture("Monk_S.tga");
    normalTexture = loadGLTexture("Monk_N_Normal_Bump.tga");
    diffuseTexture1 = loadGLTexture("D.tga");
    specularTexture1 = loadGLTexture("D_S.tga");
    normalTexture1 = loadGLTexture("D_N.tga");
    loadPlane();
    //Generate Buffes
    glGenBuffers(1, &vertexBO);
    glGenBuffers(1, &normalBO);
    
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
    
    loadShadersObjects();
    
    diffuseTextureUniformLocation = glGetUniformLocation(program, "diffuseTexture");
    specularUniformLocation = glGetUniformLocation(program, "specularTexture");
    normalUniformLocation = glGetUniformLocation(program, "normalTexture");
    
    modelviewMatrixUniformLocation = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixUniformLocation = glGetUniformLocation(program, "projectionMatrix");
    normalMatrixUniformLocation = glGetUniformLocation(program, "normalMatrix");
    
    lightPositioUniformLocation0 = glGetUniformLocation(program, "lights[0].lightPosition");
    lightPositioUniformLocation1 = glGetUniformLocation(program, "lights[1].lightPosition");
    
    lightColorUniformLocation0 = glGetUniformLocation(program, "lights[0].lightColor");
    lightColorUniformLocation1 = glGetUniformLocation(program, "lights[1].lightColor");
    
    lightSpecularColorUniformLocation0 = glGetUniformLocation(program, "lights[0].specularLightColor");
    lightSpecularColorUniformLocation1 = glGetUniformLocation(program, "lights[1].specularLightColor");
    
    positionAttribute = glGetAttribLocation(program, "position");
    normalAttribute = glGetAttribLocation(program, "normal");
    texCoordAttribute = glGetAttribLocation(program , "texCoord");
    tangentAttribute = glGetAttribLocation(program, "tangent");
    binormalAttribute = glGetAttribLocation(program, "binormal");
    
    useSpecular = glGetUniformLocation(program, "useSpecular");
    eyeMatrix = eyeMatrix.makeTranslation(Cvec3(0.0, 0.0, 45.0));
    
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
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Project-N11028457");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
    init();
    glutMainLoop();
    return 0;
}
