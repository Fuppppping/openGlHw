#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <iostream>

GLFWwindow* window;
GLint width, height;
GLfloat ratio = 1.f;
GLfloat Up = 0.0f;
GLfloat Down = 0.0f;
GLfloat Left = 0.0f;
GLfloat Right = 0.0f;
GLfloat Rotation = 0.0f;
GLfloat Move_Left_Right = 0.0f;
GLfloat Bigger = -3.0f;
GLuint texture0, texture1, texture2;

static void error_callback(int error, const char* description)
{
    return;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    if (height == 0)height = 1;                         //防止被0除
    if (h > 0) ratio = (float)w / (float)h;
    glViewport(0, 0, w, h);                             //设置观察点
    width = w;
    height = h;
    glViewport(0, 0, width, height);                    // 重设观察点
    glMatrixMode(GL_PROJECTION);                        // 选择投影矩阵
    glLoadIdentity();                                   // 重设投影矩阵观察点
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);         // 选择投影角度
    
    //GLKMatrix4MakePerspective
    glMatrixMode(GL_MODELVIEW);                         // 选择模型矩阵
    glLoadIdentity();                                   //恢复初始坐标系
}

GLuint loadBMP(const char * imagepath, GLuint  &textureID) {
    printf("Reading image %s\n", imagepath);
    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;
    unsigned char * data;   //RGB数据
    FILE * file = fopen(imagepath, "rb");
    if (!file) {
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
        getchar();
        return 0;
    }
    //读头，检查初始头是否为54byte
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file1\n");
        fclose(file);
        return 0;
    }
    //BMP文件以BM开头
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file2\n");
        fclose(file);
        return 0;
    }
    //确认是24色深度的文件
    if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file3\n");    fclose(file); return 0; }
    if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file4\n");    fclose(file); return 0; }
    //读文件的相关关系
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);
    //如果有些信息消失了
    if (imageSize == 0)    imageSize = width*height * 3;//每位3色
    if (dataPos == 0)      dataPos = 54;
    //创建读取数组
    data = new unsigned char[imageSize];
    //读取数据
    fread(data, 1, imageSize, file);
    //关闭文件
    fclose(file);
    glGenTextures(1, &textureID);
    //bind纹理
    glBindTexture(GL_TEXTURE_2D, textureID);
    //生成纹理
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    //删除
    delete[] data;
    //线性滤波
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return true;
}

void KeyBoard(){
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        Move_Left_Right -= 0.02f;
    }
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        Move_Left_Right += 0.02f;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Bigger += 0.1f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Bigger -= 0.1f;
    }
    else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        Rotation += 1.0f;
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            Down += 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            Up += 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            Left += 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            Right += 1.0f;
        }
    }
}

void Draw(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //放大缩小和平移
    glTranslatef(Move_Left_Right, 0.0f, Bigger);
    //旋转
    glRotatef(Down, -0.5f, 0.0f, 0.0f);
    glRotatef(Up, 0.5f, 0.0f, 0.0f);
    glRotatef(Right, 0.0f, 0.5f, 0.0f);
    glRotatef(Left, 0.0f, -0.5f, 0.0f);
    glRotatef(Rotation, -1.0f, -1.0f, 0.0f);
    // 前面
    glBindTexture(GL_TEXTURE_2D, texture0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // 纹理和四边形的左下
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f); // 纹理和四边形的右下
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f); // 纹理和四边形的右上
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // 纹理和四边形的左上
    // 后面
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // 纹理和四边形的右下
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // 纹理和四边形的右上
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f); // 纹理和四边形的左上
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f); // 纹理和四边形的左下
    glEnd();
    glFlush();
    // 顶面
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // 纹理和四边形的左上
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // 纹理和四边形的左下
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f); // 纹理和四边形的右下
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f); // 纹理和四边形的右上
    // 底面
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // 纹理和四边形的右上
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f); // 纹理和四边形的左上
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f); // 纹理和四边形的左下
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // 纹理和四边形的右下
    glEnd();
    glFlush();
    // 右面
    glBindTexture(GL_TEXTURE_2D, texture2);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f); // 纹理和四边形的右下
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f); // 纹理和四边形的右上
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f); // 纹理和四边形的左上
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f); // 纹理和四边形的左下
    // 左面
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // 纹理和四边形的左下
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // 纹理和四边形的右下
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // 纹理和四边形的右上
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // 纹理和四边形的左上
    glEnd();
    glFlush();
    //读取键盘设置
    KeyBoard();
}

void init_opengl(void)
{
    glShadeModel(GL_SMOOTH);                            //设置smooth颜色过渡模式
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);               //背景颜色
    glClearDepth(1.0f);                                 //设置深度
    glEnable(GL_DEPTH_TEST);                            //深度测试开启
    glDepthFunc(GL_LEQUAL);                             //如果目标像素z值<＝当前像素z值，则绘制目标像素
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //指定颜色和纹理坐标的差值质量为最高质量
}

int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    window = glfwCreateWindow(640, 480, "OpenGl-Homework1", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //设置窗口变化后的回调函数
    glfwMakeContextCurrent(window);                                    //设置为当前窗口
    glfwGetFramebufferSize(window, &width, &height);                   //获得窗口大小
    framebuffer_size_callback(window, width, height);                  //改变窗口
    glewExperimental = true;                                           //glew必须设置
    if (glewInit() != GLEW_OK)
    {
        exit(EXIT_FAILURE);
    }
    //纹理设置
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);
    loadBMP("/Users/jiaoma/gitCode/c:c++/ogl/glfw_1st_homework/OGL-1.bmp",texture0);
    loadBMP("/Users/jiaoma/gitCode/c:c++/ogl/glfw_1st_homework/OGL-2.bmp",texture1);
    loadBMP("/Users/jiaoma/gitCode/c:c++/ogl/glfw_1st_homework/OGL-3.bmp",texture2);
    //设置opengl
    init_opengl();
    do {
        Draw(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

