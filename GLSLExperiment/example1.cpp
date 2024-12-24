/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#include <ctime>

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat Theta[] = { 0, 0, 0,0,0,0,0 };
GLfloat theta[] = { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 };
GLfloat i, dr = 5;

mat4 ctm, ctm_ban, ctm_bantb, ctm_cancau, ctm_xetai, ctm_xecau, ctm_xetai1, model, phongto;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
point4 light_position(0.0, 0.0, 1.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);
color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(0.0, 1.0, 0.0, 0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;
color4 ambient_product;
color4 diffuse_product;
color4 specular_product;
color4 t;
void setMau(float ad, float bd, float cd) {
	material_diffuse = vec4(ad, bd, cd, 1.0);
	ambient_product = light_ambient * material_ambient;
	diffuse_product = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
}
void tualung() {
	setMau(0, 0, 0);
	mat4 instance = RotateX(-5) * Translate(0, 0.2, -0.14) * Scale(0.3, 0.25, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void tru() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, 0.035, -0.14) * Scale(0.02, 0.1, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void tayvinngang(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 255, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.02, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void tayvindoc(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 255, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.17, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matghe() {
	setMau(0, 0, 0);
	mat4 instance = Scale(0.3, 0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void changhe() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, -0.06, 0) * Scale(0.02, 0.12, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi1() {
	setMau(0, 0, 0);
	mat4 instance = RotateX(40) * Translate(0, -0.135, 0.06) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi2() {
	setMau(0, 0, 0);
	mat4 instance = RotateX(-40) * Translate(0, -0.135, -0.06) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi3() {
	setMau(0, 0, 0);
	mat4 instance = RotateZ(-40) * Translate(0.06, -0.135, 0) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi4() {
	setMau(0, 0, 0);
	mat4 instance = RotateZ(40) * Translate(-0.06, -0.135, 0) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ghe1() {
	mat4 vitri = Translate(-2, -1, -2.5);
	phongto = Scale(3, 3, 3);
	ctm = vitri * phongto;
	changhe();
	chanduoi1();
	chanduoi2();
	chanduoi3();
	chanduoi4();
	ctm = vitri * phongto * RotateX(Theta[0]) * RotateY(Theta[1]) * Translate(0, i, 0);
	tualung();
	tru();
	matghe();
	tayvinngang(0.14, 0.155, -0.075);
	tayvinngang(-0.14, 0.155, -0.075);
	tayvindoc(0.14, 0.08, 0);
	tayvindoc(-0.14, 0.08, 0);
}
void matban() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, 0, 0) * Scale(0.6, 0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanban(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.03, 0.3, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void demt() {
	setMau(0, 255, 255);
	mat4 instance = Translate(0, 0.015, 0) * Scale(0.15, 0.03, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void trumt() {
	setMau(0, 255, 255);
	mat4 instance = Translate(0, 0.045, 0) * Scale(0.03, 0.06, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void manhinhmt() {
	setMau(0, 255, 255);
	mat4 instance = RotateX(5) * Translate(0, 0.19, 0) * Scale(0.3, 0.25, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ongbut() {
	setMau(255, 0, 255);
	mat4 instance = Translate(0.25, 0.05, 0.1) * Scale(0.05, 0.1, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void but1() {
	setMau(255, 0, 0);
	mat4 instance = RotateX(10) * Translate(0.25, 0.15, 0.1) * Scale(0.01, 0.1, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void but2() {
	setMau(0, 255, 255);
	mat4 instance = RotateX(-10) * Translate(0.25, 0.1, 0.1) * Scale(0.01, 0.13, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void but3() {
	setMau(0, 255, 0);
	mat4 instance = RotateX(10) * Translate(0.24, 0.1, 0.1) * Scale(0.01, 0.13, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chuot() {
	setMau(255, 0, 255);
	mat4 instance = Translate(-0.24, 0.01, 0) * Scale(0.03, 0.03, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void banthungan() {
	mat4 vitri = Translate(-2, -1.5, -1);
	ctm_ban = vitri * phongto;
	matban();
	chanban(0.285, -0.15, -0.135);
	chanban(-0.285, -0.15, 0.135);
	chanban(-0.285, -0.15, -0.135);
	chanban(0.285, -0.15, 0.135);
	demt();
	trumt();
	manhinhmt();
	ongbut();
	but1();
	but2();
	but3();
	chuot();
}
void matbantb() {
	setMau(0, 0, 0);
	mat4 instance = Scale(0.7, 0.02, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bantb * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanbantb(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.03, 0.3, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bantb * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void bantrungbay() {
	mat4 vitri = Translate(2, -1, -1);
	ctm_bantb = vitri * phongto;
	matbantb();
	chanbantb(0.335, -0.15, 0.185);
	chanbantb(0.335, -0.15, -0.185);
	chanbantb(-0.335, -0.15, -0.185);
	chanbantb(-0.335, -0.15, 0.185);
}
void banhxe(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 255, 0);
	for (int z = 0; z < 360; z += 1) {
		mat4 instance = RotateX(90) * Translate(a, b, c) * RotateY(z) * Scale(0.1, 0.05, 0.1);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
void trucxe(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 255, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.05, 0.05, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void thungxe() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0.15, 0.05, 0.2) * Scale(0.6, 0.1, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void mat1(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.2, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matday() {
	setMau(0, 0, 255);
	mat4 instance = Translate(0.325, 0.11, 0.2) * Scale(0.25, 0.02, 0.39);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matben(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.22, 0.17, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matcua(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.2, 0.39);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cuatrai(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.26, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cuaphai(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.26, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void mattren() {
	setMau(255, 0, 255);
	mat4 instance = Translate(0.02, 0.25, 0.2) * Scale(0.28, 0.02, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
GLfloat m = 0, n;

void xetai() {
	mat4 vitri = Translate(2, -0.8, -1);
	ctm_xetai1 = vitri * Translate(m, 0, 0);
	banhxe(0, 0, 0);
	banhxe(0.3, 0, 0);
	banhxe(0, 0.4, 0);
	banhxe(0.3, 0.4, 0);
	trucxe(0, 0, 0.2);
	trucxe(0.3, 0, 0.2);
	thungxe();
	matben(0.31, 0.17, 0);
	matben(0.31, 0.17, 0.4);
	matday();
	mat1(0.2, 0.15, 0.2);
	mat1(0.43, 0.15, 0.2);
	mat1(0.15, 0.15, 0.2);
	matcua(-0.12, 0.15, 0.2);
	cuatrai(0.02, 0.15, 0);
	mattren();
	ctm_xetai1 = ctm_xetai1 * Translate(-0.14, 0, 0.4) * RotateY(Theta[3]);
	cuaphai(0.15, 0.14, -0.01);
}
void banhxecau(GLfloat a, GLfloat b, GLfloat c) {
	for (int z = 0; z < 360; z += 1) {
		mat4 instance = RotateX(90) * Translate(a, b, c) * RotateY(z) * Scale(0.1, 0.05, 0.1);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
void thanxecau() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0.15, 0.1, 0.2) * Scale(0.6, 0.15, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void dauxecau() {
	setMau(0, 0, 255);
	mat4 instance = Translate(-0.01, 0.2, 0.2) * Scale(0.25, 0.2, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void duoixecau() {
	setMau(255, 255, 0);
	mat4 instance = Translate(0, 0.3, 0) * Scale(0.3, 0.23, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void truccau() {
	setMau(0, 255, 0);
	mat4 instance = RotateZ(-15) * Translate(0, 0.62, 0) * Scale(0.09, 0.4, 0.09);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void khucnoi() {
	setMau(255, 0, 0);
	mat4 instance = RotateZ(-20) * Translate(-0.01, 0.18, 0) * Scale(0.06, 0.4, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void day() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0.14, 0.18, 0) * Scale(0.02, 0.4, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cuchang() {
	setMau(0, 0, 255);
	mat4 instance = Translate(0.14, 0, 0) * Scale(0.07, 0.07, 0.07);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cancau() {
	mat4 vitri = Translate(1, -0.8, -0.5);
	ctm_xecau = vitri;
	banhxecau(0, 0, 0);
	banhxecau(0.3, 0, 0);
	banhxecau(0, 0.4, 0);
	banhxecau(0.3, 0.4, 0);
	thanxecau();
	dauxecau();
	ctm_xecau = ctm_xecau * Translate(0.3, 0, 0.2) * RotateY(Theta[4]);
	duoixecau();
	truccau();
	ctm_xecau = ctm_xecau * Translate(0.22, 0.78, 0) * RotateZ(Theta[5]);
	khucnoi();
	day();
	cuchang();

}
/* LÊ THị TRANG - ROBOT*/
void torso()
{
	setMau(0, 0, 255);
	mat4 instance = Translate(0, -0.35, 0) * Scale(0.6, 0.7, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void head()
{
	setMau(0, 0, 0);
	mat4 instance = Translate(0, 0.075, 0) * Scale(0.2, 0.15, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void upper_arm()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.1, 0.3, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void lower_arm()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.05, 0.3, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void upper_leg()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.2, 0.45, 0.25);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void lower_leg()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.15, 0.35, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
mat4 trans, thunho;

void robot()
{
	mat4 vitri = Translate(-0.5, -1.5, -2);

	model = vitri * Translate(0, 0.75, 0) * RotateY(theta[0]);
	trans = model;
	torso();
	model = trans * RotateY(theta[1]);
	head();
	model = trans * Translate(0, -0.05, 0) * RotateX(theta[2]) * Translate(-0.35, -0.15, 0);
	upper_arm();
	model = model * Translate(0, -0.15, 0) * RotateX(theta[3]) * Translate(0, -0.15, 0);
	lower_arm();
	model = trans * Translate(0, -0.05, 0) * RotateX(theta[4]) * Translate(0.35, -0.15, 0);
	upper_arm();
	model = model * Translate(0, -0.15, 0) * RotateX(theta[5]) * Translate(0, -0.15, 0);
	lower_arm();
	model = trans * Translate(0, -0.7, 0) * RotateX(theta[6]) * Translate(-0.15, -0.225, 0);
	upper_leg();
	model = model * Translate(0, -0.225, 0) * RotateX(theta[7]) * Translate(0, -0.175, 0);
	lower_leg();
	model = trans * Translate(0, -0.7, 0) * RotateX(theta[8]) * Translate(0.15, -0.225, 0);
	upper_leg();
	model = model * Translate(0, -0.225, 0) * RotateX(theta[9]) * Translate(0, -0.175, 0);
	lower_leg();
}
/* END - ROBOT*/


/* LÊ THẢO VÂN - TỦ TRƯNG BÀY*/
mat4 ctm_oto;
mat4 ctm_tu;
float ctm_tu_delta_y = -0.1;
void banh(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	for (int z = 0; z < 360; z += 1) {
		mat4 instance = RotateX(90) * Translate(a, b, c) * RotateY(z) * Scale(0.09, 0.05, 0.09);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
void dayoto() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0.15, 0.1, 0.15) * Scale(0.4, 0.1, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void dauoto() {
	setMau(255, 255, 0);
	mat4 instance = Translate(0.02, 0.175, 0.15) * Scale(0.15, 0.15, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void duoioto() {
	setMau(0, 255, 255);
	mat4 instance = Translate(0.24, 0.175, 0.15) * Scale(0.2, 0.25, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void oto1() {
	mat4 vitri = Translate(0.2, 0.18 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void oto2() {
	mat4 vitri = Translate(0.7, 0.18 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void oto3() {
	mat4 vitri = Translate(0.7, 1.05 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void oto4() {
	mat4 vitri = Translate(0.2, 1.05 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void mattraitu(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0.1, 0.5, 0.2);
	mat4 instance = Translate(a, b, c) * Scale(0.03, 1, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void mattrentu(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.6, 0.03, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ke(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.2, 0.05, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void tu() {
	mat4 vitri = Translate(0, 0.55 + ctm_tu_delta_y, 1.5);
	ctm_tu = vitri * RotateY(Theta[2]);
	mattraitu(0, 0, 0);
	mattraitu(0.6, 0, 0);
	mattraitu(0.3, 0, 0);
	mattrentu(0.3, 0, 0);
	mattrentu(0.3, 0.485, 0);
	mattrentu(0.3, -0.485, 0);
	ke(0.15, 0.03, -0.15);
	ke(0.45, 0.03, -0.15);
	ke(0.45, -0.45, -0.15);
	ke(0.15, -0.45, -0.15);
	oto1();
	oto2();
	oto3();
	oto4();
}

void tu2() {
	mat4 vitri = Translate(-0.7, 0.55 + ctm_tu_delta_y, 1.5);
	ctm_tu = vitri * RotateY(Theta[2]);
	mattraitu(0, 0, 0);
	mattraitu(0.6, 0, 0);
	mattraitu(0.3, 0, 0);
	mattrentu(0.3, 0, 0);
	mattrentu(0.3, 0.485, 0);
	mattrentu(0.3, -0.485, 0);
	ke(0.15, 0.03, -0.15);
	ke(0.45, 0.03, -0.15);
	ke(0.45, -0.45, -0.15);
	ke(0.15, -0.45, -0.15);
}
/* END TỦ TRUNG BÀY*/


/*TÊN LỬA*/
double cao_than = 2, rong_than = 0.5;
double dai_canh = 0.5, rong_canh = 0.3, day_canh = 0.02;
double de = 1;
mat4 model1;
void ten_lua() {
	setMau(0, 0, 255);
	//than ten lua
	mat4 instance = Translate(0, cao_than / 2, 0) * Scale(rong_than, cao_than, rong_than);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//dau ten lua
	instance = Translate(0, cao_than, 0) * Scale(rong_than / 1.4, rong_than / 1.4, rong_than) * RotateZ(45);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh trai
	setMau(0, 255, 255);
	instance = Translate(rong_than - rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh phai
	setMau(0, 255, 255);
	instance = Translate(-rong_than + rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh trai
	setMau(0, 255, 255);
	instance = RotateY(90) * Translate(rong_than - rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh phai
	setMau(0, 255, 255);
	instance = RotateY(90) * Translate(-rong_than + rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void de_be_phong_nho() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, de / 40, 0) * Scale(de / 2, de / 20, de / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void de_be_phong_to() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0, 3 * de / 40, 0) * Scale(cao_than, de / 10, de);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
double yy = 0;
double alpha = 0, beta = 0, gama = 0;
void gia_do_ten_lua() {
	setMau(255, 255, 0);
	mat4 instance = Scale(cao_than + 1.5 * rong_canh, de / 5, de);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(cao_than / 2 - rong_than / 1.5, de / 5, 0) * Scale(de / 20, rong_canh, 3 * de / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void le_be() {
	mat4 instance = Scale(0.01, 0.01, de);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void be_phong() {
	mat4 vitri = Translate(0.2, -1, -1);
	mat4 thunho = Scale(0.25, 0.25, 0.25);
	model1 = vitri * thunho * Translate(0, -0.6, 0);
	de_be_phong_nho();
	model1 *= RotateY(alpha);
	de_be_phong_to();
	model1 *= Translate(-cao_than / 2, 3 * de / 40 + de / 10, 0) * RotateZ(beta);
	le_be();
	model1 *= Translate((cao_than + 1.5 * rong_canh) / 2, 0, 0);
	gia_do_ten_lua();
	model *= RotateZ(-90 + gama) * Translate(-rong_than / 2 - rong_canh, -(cao_than + 1.5 * rong_canh) / 2 + yy, 0);
	ten_lua();

}

void spinArm(void)
{
	theta[2] += 0.01;
	if (theta[2] > 60) theta[2] -= 120;
	theta[4] -= 0.01;
	if (theta[4] < -60) theta[4] += 120;
	theta[6] += 0.01;
	if (theta[6] > 30) theta[6] -= 60;
	theta[7] = 0;
	theta[8] -= 0.01;
	if (theta[8] < -30) theta[8] += 60;
	theta[9] = 0;
	glutPostRedisplay();
}
void sit(void)
{
	theta[6] = 90;
	theta[7] = -90;
	theta[8] = 90;
	theta[9] = -90;
	glutPostRedisplay();
}
void unsit(void)
{
	theta[2] = 0;
	theta[4] = 0;
	theta[6] = 0;
	theta[7] = 0;
	theta[8] = 0;
	theta[9] = 0;
	glutPostRedisplay();
}

// BHV

GLfloat fanRotate = 0;
bool isRorate = true;
float quat_y = -0.3;
void fan_display() {
	setMau(0.4, 0.1, 0.01);
	mat4 instance = Translate(0, 2 + quat_y, 0.08);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(0.1, 0.3 + quat_y, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	if (isRorate) {
		fanRotate += 10;
	}

	setMau(0.01, 0.5, 0.9);
	float scale = 0.4;
	instance = Translate(cos(fanRotate * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate * DegreesToRadians) * scale) * RotateY(-fanRotate);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	GLfloat fanRotate1 = fanRotate + 90;
	instance = Translate(cos(fanRotate1 * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate1 * DegreesToRadians) * scale) * RotateY(-fanRotate1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	fanRotate1 += 90;
	instance = Translate(cos(fanRotate1 * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate1 * DegreesToRadians) * scale) * RotateY(-fanRotate1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	fanRotate1 += 90;
	instance = Translate(cos(fanRotate1 * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate1 * DegreesToRadians) * scale) * RotateY(-fanRotate1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	glutPostRedisplay();
}


float dong_ho_x = 0, dong_ho_y = -0.5;

void dong_ho1()
{
	mat4 model_dong_ho = Translate(1 + dong_ho_x, 1 + dong_ho_y, 0) * Scale(1, 1, 1);
	setMau(0.3, 0.4, 0.5);
	mat4 instance = model_dong_ho * Translate(1 + dong_ho_x, 1 + dong_ho_y, 0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 1, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	float scale = 0.4;

	for (int i = 0; i < 12; i++)
	{
		float angle = i * 30 * DegreesToRadians;
		setMau(0.8, 0.4, 0.5);
		instance = model_dong_ho * Translate(sin(angle) * scale, cos(angle) * scale, 0) * RotateZ(i * -30);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.1, 0.2));
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


	time_t now = time(0);
	tm* localTime = localtime(&now);

	int hours = localTime->tm_hour % 12;  // Lấy giờ, giới hạn từ 0 đến 11
	int minutes = localTime->tm_min;     // Lấy phút
	int seconds = localTime->tm_sec;     // Lấy giây

	// Tính góc quay cho từng kim
	float hourAngle = 30.0f * hours - (minutes / 2.0f);  // 360° / 12 = 30° mỗi giờ
	float minuteAngle = 6.0f * minutes;                 // 360° / 60 = 6° mỗi phút
	float secondAngle = 6.0f * seconds;

	scale = 0.4;
	instance = model_dong_ho * Translate(sin(secondAngle * DegreesToRadians) * 0.1, cos(secondAngle * DegreesToRadians) * 0.1, 0) * RotateZ(-secondAngle);
	setMau(0.7, 0.4, 0.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.5, 0.2));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	scale = 0.4;
	instance = model_dong_ho * Translate(sin(minuteAngle * DegreesToRadians) * 0.1, cos(minuteAngle * DegreesToRadians) * 0.1, 0) * RotateZ(-minuteAngle);
	setMau(0.1, 0.7, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.4, 0.2));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	scale = 0.4;
	instance = model_dong_ho * Translate(sin(hourAngle * DegreesToRadians) * 0.1, cos(hourAngle * DegreesToRadians) * 0.1, 0) * RotateZ(-hourAngle);
	setMau(0.1, 0.0, 0.8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.4, 0.2));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	glutPostRedisplay();
}


void tuongBenTrai() {
	setMau(0.8, 0.8, 0.8); // Màu xám nhạt
	mat4 instance = Translate(-4.0, 0.5, 0) * Scale(0.05, 5.0, 5.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuongPhiaSau() {
	setMau(0.7, 0.7, 0.7); // Màu xám đậm hơn
	mat4 instance = Translate(0.0, 0.5, -2.0) * Scale(10.0, 5.0, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuongBenTren() {
	setMau(0.9, 0.9, 0.9); // Màu sáng hơn
	mat4 instance = Translate(0.0, 3.0, 0.0) * Scale(10.0, 0.05, 5.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong() {
	tuongBenTrai();
	tuongPhiaSau();
	tuongBenTren();
	glutPostRedisplay();
}
void Phong(color4 light_ambient, color4 light_diffuse, color4 light_specular,
	color4 material_ambient, color4 material_diffuse, color4 material_specular,
	vec4 light_position, GLfloat material_shininess) {

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
GLfloat mauAnhSang = 1.0; //Màu của ánh sáng
void paintColor(GLfloat a, GLfloat b, GLfloat c) {
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 1.9, 0.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(mauAnhSang, mauAnhSang, mauAnhSang, 1.0); //màu ánh sáng
	color4 light_specular(0.0, 0.0, 0.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(a / 255.0, b / 255.0, c / 255.0, 1.0);  //màu bóng
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
/* ------------------------------------ DỰNG HÌNH CĂN PHÒNG  ------------------------------------ */
mat4 model_room, ctm11;
GLfloat xoay_roomx = 0, xoay_roomy = 0, xoay_roomz = 0;

void san()
{
	mat4 instance = Translate(0.75f, 0.01f, -1.0f) * Scale(1.5f, 0.02f, 2.0f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong_sau()
{
	mat4 instance = Translate(0.75f, 0.4f, -0.01f) * Scale(1.5f, 0.8f, 0.02f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong_trai()
{
	mat4 instance = Translate(-0.01f, 0.4f, -1.0f) * Scale(0.02f, 0.8f, 2.0f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tuong_duoi1()
{
	mat4 instance = Translate(0.01f, -0.4f, -1.0f) * Scale(0.02f, 0.8f, 2.0f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tuong_duoi2()
{
	mat4 instance = Translate(0.75f, -0.4f, -0.01f) * Scale(1.5f, 0.8f, 0.02f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong_duoi3()
{
	mat4 instance = Translate(0.75f, -0.4f, -2.0f) * Scale(1.5f, 0.8f, 0.02f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tuong_tren3()
{
	mat4 instance = Translate(0.75f, 0.4f, -2.0f) * Scale(1.5f, 0.8f, 0.02f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void build_room()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.0, 1.0, 0.0, 1.0),   // material_ambient (green)
		color4(0.0, 0.8, 0.0, 1.0),   // material_diffuse (green)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	san();
	tuong_sau();
	tuong_trai();
	tuong_tren3();
	tuong_duoi1();
	tuong_duoi2();
	tuong_duoi3();
}

/* ------------------------------------ QUẠT TRẦN ------------------------------------ */
mat4 model_quat;
GLfloat xoay_quaty = 0; //điều khiển quạt

void truc_dai()
{
	mat4 instance = Scale(0.01f, 0.1f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11* model_quat * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void truc_quay()
{
	mat4 instance = Translate(0.0f, -0.065f, 0.0f) * Scale(0.01f, 0.01f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11* model_quat * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_quat(GLfloat x)
{
	mat4 instance = RotateY(x) * Translate(0.08f, -0.065f, 0.0f) * Scale(0.15f, 0.01f, 0.03f);
	glUniformMatrix4fv(model_loc , GL_TRUE, 1, ctm11 * model_quat * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLboolean checked = false;//Kiểm tra quạt có hoạt động hay không

void canh_quay()
{
	if (checked) {
		xoay_quaty += 2;
		truc_quay();
		canh_quat(0);
		canh_quat(120);
		canh_quat(-120);
		glutPostRedisplay();
	}
	else {
		truc_quay();
		canh_quat(0);
		canh_quat(120);
		canh_quat(-120);
	}
}

void quat_tran()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 1.5, 2.0, 1.0),   // material_ambient (xanh biển)
		color4(0.5, 1.5, 2.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	model_quat = model_room * Translate(0.75f, 0.75f, -1.0f) * Scale(2.0f);
	truc_dai();
	model_quat = model_quat * RotateY(xoay_quaty);
	canh_quay();
}

/* ------------------------------------	 BÀN LÀM VIỆC  ------------------------------------ */
//vẽ bàn
mat4 model_ban;
GLfloat di_chuyen_banx = 0, di_chuyen_bany = 0, di_chuyen_banz = 0;
void mat_ban()
{
	mat4 instance = Translate(1.2f, 0.215f, -0.2f) * Scale(0.5f, 0.01f, 0.3f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chan_ban(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.2f, 0.3f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_day(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.2f, 0.01f, 0.3f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void khung_ban()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.05, 0.5, 0.9, 1.0),   // material_ambient (xanh biển)
		color4(0.05, 0.5, 0.9, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat_ban();
	chan_ban(1.445f, 0.11f, -0.2f);
	chan_ban(1.15f, 0.11f, -0.2f);
	chan_ban(0.95f, 0.11f, -0.2f);
	thanh_day(1.05f, 0.05f, -0.2f);
}

//cánh tủ bàn
mat4 model_canh_tu_ban;
GLfloat mo_canh_tu_ban = 0;

void canh(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.1, 0.0, 1.0, 1.0),   // material_ambient (hồng)
		color4(1.1, 0.0, 1.0, 1.0),   // material_diffuse (hồng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.19f, 0.08f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_canh_tu_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tay_cam_tu_ban(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(-0.5, 0.0, 1.0, 1.0),   // material_ambient (xanh biển)
		color4(-0.5, 0.0, 1.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.02f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_canh_tu_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_tu_ban()
{
	canh(1.05f, 0.09f, -0.35f);
	tay_cam_tu_ban(1.12f, 0.075f, -0.36f);

}

//vẽ ngăn kéo
mat4 model_ngan_keo;
GLfloat keo_ngan_keo_banx = 0, keo_ngan_keo_bany = 0, keo_ngan_keo_banz = 0;

void thanh_ngan(GLfloat x, GLfloat y, GLfloat z) {
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.1, 0.0, 1.0, 1.0),   // material_ambient (hồng)
		color4(1.1, 0.0, 1.0, 1.0),   // material_diffuse (hồng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.19f, 0.075f, 0.01);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_dai(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.1, 0.0, 1.0, 1.0),   // material_ambient (hồng)
		color4(1.1, 0.0, 1.0, 1.0),   // material_diffuse (hồng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.075f, 0.3f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void day(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.1, 0.0, 1.0, 1.0),   // material_ambient (hồng)
		color4(1.1, 0.0, 1.0, 1.0),   // material_diffuse (hồng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.185f, 0.01f, 0.295f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tay_cam(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(-0.5, 0.0, 1.0, 1.0),   // material_ambient (xanh biển)
		color4(-0.5, 0.0, 1.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.04f, 0.01f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void khung_ngan_keo()
{
	thanh_ngan(1.05f, 0.1725f, -0.05f);
	thanh_ngan(1.05f, 0.1725f, -0.35f);
	thanh_dai(1.145f, 0.1725f, -0.2f);
	thanh_dai(0.955f, 0.1725f, -0.2f);
	day(1.05f, 0.135f, -0.1975f);
	tay_cam(1.05f, 0.1725f, -0.355f);
}

void ngan_keo()
{
	khung_ngan_keo();
}

//vẽ máy tính
void man_hinh(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.8, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.8, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.2f, 0.15f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void than_may_tinh(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.5, 1, 1.0),   // material_ambient (xanh biển)
		color4(0.5, 0.5, 1, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.02f, 0.05f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chan_may_tinh(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (hồng)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (hồng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.06f, 0.01f, 0.03f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void may_tinh()
{
	chan_may_tinh(1.3f, 0.225f, -0.1f);
	than_may_tinh(1.3f, 0.25f, -0.1f);
	man_hinh(1.3f, 0.325f, -0.1f);
}

void ban_phim(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.1, 0.0, 1.0, 1.0),   // material_ambient (hồng)
		color4(1.1, 0.0, 1.0, 1.0),   // material_diffuse (hồng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.25f, 0.01f, 0.08f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cay_may_tinh(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.5, 1, 1.0),   // material_ambient (xanh biển)
		color4(0.5, 0.5, 1, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.05f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void ban_lam_viec()
{
	model_ban = model_room * Translate(di_chuyen_banx, di_chuyen_bany, di_chuyen_banz);
	khung_ban();
	may_tinh();
	ban_phim(1.3f, 0.225f, -0.2f);
	cay_may_tinh(1.1f, 0.27f, -0.15f);
	model_canh_tu_ban = model_ban * Translate(0.955f, 0.09f, -0.35f) * RotateY(mo_canh_tu_ban) * Translate(-0.955f, -0.09f, 0.35f);
	canh_tu_ban();
	model_ngan_keo = model_ban * Translate(keo_ngan_keo_banx, keo_ngan_keo_bany, keo_ngan_keo_banz);
	ngan_keo();
}

/* ------------------------------------	 GHẾ TỰA  ------------------------------------ */
mat4 model_ghe;
GLfloat di_chuyenx = 0, di_chuyeny = 0, di_chuyenz = 0, quay_ghe = 0;
GLfloat nang_ghey = 0;
void de_ghe(GLfloat x, GLfloat y, GLfloat z, GLfloat i)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.12f, 0.01f, 0.12f) * RotateY(i);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chan_ghe(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.15f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cho_ngoi(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.2f, 0.01f, 0.2f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tua(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.2f, 0.3f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void de_tay(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.02f, 0.01f, 0.2f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ghe()
{
	model_ghe = model_room * Translate(di_chuyenx, di_chuyeny, di_chuyenz);

	for (int i = 0; i < 1000; i++) {
		de_ghe(1.3f, 0.02f, -0.5f, i);
	}
	chan_ghe(1.3f, 0.095f, -0.5f);

	model_ghe = model_ghe * Translate(0.0f, nang_ghey, 0.0f) * Translate(1.3f, 0.02f, -0.5f) * RotateY(quay_ghe) * Translate(-1.3f, -0.02f, 0.5f);
	cho_ngoi(1.3f, 0.175f, -0.5f);
	tua(1.3f, 0.325f, -0.595f);
	de_tay(1.39f, 0.25f, -0.495f);
	de_tay(1.21f, 0.25f, -0.495f);
}

/* ------------------------------------	 TRANH  ------------------------------------ */
mat4 model_tranh;
GLfloat di_chuyen_tranhz = 0;

void nen_tranh(GLfloat x, GLfloat y, GLfloat z)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.8, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.8, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.3f, 0.6f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tranh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh1(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.0, 0.0, 1.0),   // material_ambient (xanh biển)
		color4(1.0, 0.0, 0.0, 1.0),   // material_diffuse (xanh biển)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * RotateX(a) * Scale(0.03f, 0.1f, 0.03f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tranh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tranh()
{
	model_tranh = model_room * Translate(0.0f, 0.05f, di_chuyen_tranhz - 0.45);
	nen_tranh(0.0f, 0.55f, -0.75f);
	thanh1(0.01f, 0.55f, -0.9f, 30);
	thanh1(0.01f, 0.55f, -0.9f, -30);
	thanh1(0.01f, 0.55f, -0.6f, -30);
	thanh1(0.01f, 0.55f, -0.6f, 30);
	thanh1(0.01f, 0.55f, -0.75f, -30);
	thanh1(0.01f, 0.55f, -0.75f, 30);
}

/* ------------------------------------	 TỦ QUẦN ÁO  ------------------------------------ */
mat4 model_tu_quan_ao;

GLfloat di_chuyen_tu_quan_aox = 0, di_chuyen_tu_quan_aoy = 0, di_chuyen_tu_quan_aoz = 0;

void mat_sau(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.8f, 0.6f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11* model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chan_tu(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.6f, 0.3f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void mat_tren(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.8f, 0.01f, 0.3f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_treo(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.79f, 0.01f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_doc(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.05f, 0.6f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_ngang(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.7f, 0.03f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void khung_tu_quan_ao()
{
	mat_sau(0.42f, 0.3f, -0.05f);
	chan_tu(0.815f, 0.3f, -0.205f);
	chan_tu(0.025f, 0.3f, -0.205f);
	mat_tren(0.42f, 0.6f, -0.205f);

	thanh_treo(0.42f, 0.45f, -0.205f);

	mat_tren(0.42f, 0.2f, -0.205f);

	thanh_doc(0.795f, 0.3f, -0.36f);
	thanh_doc(0.045f, 0.3f, -0.36f);

	thanh_ngang(0.42f, 0.2f, -0.36f);
	thanh_ngang(0.42f, 0.585f, -0.36f);
}

//vẽ cánh tủ quần áo
mat4 model_canh_trai;
mat4 model_canh_phai;

GLfloat quay_canh_trai_tu_quan_ao = 0;
GLfloat quay_canh_phai_tu_quan_ao = 0;

void canh_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z, mat4 modelx)
{
	mat4 instance = Translate(x, y, z) * Scale(0.35f, 0.37f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * modelx * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tay_cam_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z, mat4 modelx)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.04f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * modelx * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_trai()
{
	paintColor(0, 250, 100);
	canh_tu_quan_ao(0.245f, 0.39f, -0.36f, model_canh_trai);
	tay_cam_tu_quan_ao(0.4f, 0.39f, -0.365f, model_canh_trai);
}

void canh_phai()
{
	paintColor(0, 250, 100);
	canh_tu_quan_ao(0.595f, 0.39f, -0.36f, model_canh_phai);
	tay_cam_tu_quan_ao(0.44f, 0.39f, -0.365f, model_canh_phai);
}

// vẽ ngăn kéo tủ quần áo
mat4 model_ngan_keo_tu_quan_ao;

GLfloat di_chuyen_ngan_keo_tu_quan_aoz = 0;

void thanh_ngang_ngan_keo(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.7f, 0.185f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_doc_ngan_keo(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.185f, 0.24f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11* model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void day_ngan_keo_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.68f, 0.01f, 0.23f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tay_cam_ngan_keo_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.04f, 0.01f, 0.01f);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ngan_keo_tu_quan_ao()
{
	thanh_ngang_ngan_keo(0.42f, 0.1025f, -0.36f);
	thanh_ngang_ngan_keo(0.42f, 0.1025f, -0.13f);
	day_ngan_keo_tu_quan_ao(0.42f, 0.01f, -0.245f);
	thanh_doc_ngan_keo(0.765f, 0.1025f, -0.245f);
	thanh_doc_ngan_keo(0.08f, 0.1025f, -0.245f);
	tay_cam_ngan_keo_tu_quan_ao(0.42f, 0.1025f, -0.37f);
}

void tu_quan_ao()
{
	model_tu_quan_ao = model_room * Translate(di_chuyen_tu_quan_aox, di_chuyen_tu_quan_aoy, di_chuyen_tu_quan_aoz) * Translate(0.0f, 0.01f, 0.0f);
	khung_tu_quan_ao();
	model_canh_trai = model_tu_quan_ao * Translate(0.07f, 0.39f, -0.36f) * RotateY(quay_canh_trai_tu_quan_ao) * Translate(-0.07f, -0.39f, 0.36f);
	canh_trai();
	model_canh_phai = model_tu_quan_ao * Translate(0.77f, 0.39f, -0.36f) * RotateY(quay_canh_phai_tu_quan_ao) * Translate(-0.77f, -0.39f, 0.36f);
	canh_phai();
	model_ngan_keo_tu_quan_ao = model_tu_quan_ao * Translate(0.0f, 0.0f, di_chuyen_ngan_keo_tu_quan_aoz);
	ngan_keo_tu_quan_ao();
}


/* ------------------------------------	 GIƯỜNG NGỦ  ------------------------------------ */
mat4 model_giuong, model_goi1, model_goi2;

GLfloat xoay_demY = 0, xoay_demX = 0, goi1 = 0, goi2 = 0;
GLfloat  zz = 0, yy1 = 0, xx = 0, aa = 0, di_chuyen_demX = 0, di_chuyen_demZ = 0, di_chuyen_demY = 0;

void ThanGiuong()
{
	mat4 instance = Translate(0, 0, 0) * Scale(0.8, 0.2, 0.8);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_giuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ChanGiuong(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.05, 0.25, 0.05);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_giuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ThanhGiuong(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.8, 0.2, 0.05);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_giuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void DemGiuong()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.0, 0.5, 1.0, 1.0),   // material_ambient (xanh duong)
		color4(0.0, 0.5, 1.0, 1.0),   // material_diffuse (xanh duong)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(0, 0.125, 0) * Scale(0.76, 0.05, 0.76);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_giuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Goi1(GLfloat x, GLfloat y, GLfloat z)
{

	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.0, 0.5, 1.0),   // material_ambient (tim)
		color4(0.5, 0.0, 0.5, 1.0),   // material_diffuse (tim)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.25, 0.05, 0.15);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_goi1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Goi2(GLfloat x, GLfloat y, GLfloat z)
{

	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.0, 0.5, 1.0),   // material_ambient (tim)
		color4(0.5, 0.0, 0.5, 1.0),   // material_diffuse (tim)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * Scale(0.25, 0.05, 0.15);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_goi2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void KhungGiuong()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.8, 0.0, 1.0),   // material_ambient (vàng)
		color4(1.0, 0.8, 0.0, 1.0),   // material_diffuse (vàng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	ThanGiuong();
	ThanhGiuong(0, 0.15, 0.4);
	ChanGiuong(0.39, -0.05, 0.4);
	ChanGiuong(-0.39, -0.05, 0.4);
	ChanGiuong(0.39, -0.05, -0.4);
	ChanGiuong(-0.39, -0.05, -0.4);
}
void Giuong()
{
	model_giuong = model_room * Translate(xx + 0.4, yy1 + 0.2, zz - 1.2) * Scale(0.9) * RotateY(aa - 90);
	KhungGiuong();
	model_giuong = model_giuong * Translate(di_chuyen_demX, di_chuyen_demY, di_chuyen_demZ) * Translate(0, 0.1, -di_chuyen_demZ - 0.4) * RotateX(xoay_demX) * Translate(0, -0.1, di_chuyen_demZ + 0.4) * RotateY(xoay_demY);
	DemGiuong();
	model_goi1 = model_giuong * Translate(0, 0, goi1);
	model_goi2 = model_giuong * Translate(0, 0, goi2);
	Goi1(0.2, 0.175, 0.25);
	Goi2(-0.2, 0.175, 0.25);
}

/* ------------------------------------	 TỦ GIƯỜNG NGỦ  ------------------------------------ */
mat4 model_tuDen;
GLfloat  zzz = 0, yyy = 0, xxx = 0, aaa = 0, bb = 0;

GLfloat NKTuGiong = 0;
mat4 model_NKTuGiuong, model_CanhTuGiong;

void DayTu(GLfloat x)
{
	mat4 instance = Translate(0, x, 0) * Scale(0.2, 0.01, 0.2);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tuDen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void ThanhTu(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	mat4 instance = Translate(x, y, z) * RotateY(a) * Scale(0.2, 0.3, 0.01);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tuDen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void CanhTu()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.6, 0.0, 1.0),   // material_ambient (cam)
		color4(1.0, 0.6, 0.0, 1.0),   // material_diffuse (cam)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(0, 0.05, -0.1) * Scale(0.2, 0.2, 0.01);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_CanhTuGiong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ChanTu(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.02, 0.3, 0.02);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_tuDen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void TayCam(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.0, 0.5, 1.0),   // material_ambient (tim)
		color4(0.5, 0.0, 0.5, 1.0),   // material_diffuse (tim)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * RotateZ(a) * Scale(0.01, 0.05, 0.02);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_CanhTuGiong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ThanTu()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.8, 0.0, 1.0),   // material_ambient (vàng)
		color4(1.0, 0.8, 0.0, 1.0),   // material_diffuse (vàng)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	DayTu(0);
	DayTu(0.06);
	DayTu(0.12);
	DayTu(0.24);
	ThanhTu(0, 0.1, 0.1, 0);
	ThanhTu(0.1, 0.1, 0, 90);
	ThanhTu(-0.1, 0.1, 0, 90);
	ChanTu(0.095, 0.085, 0.095);
	ChanTu(-0.095, 0.085, -0.093);
	ChanTu(-0.095, 0.085, 0.095);
	ChanTu(0.095, 0.085, -0.093);
}


void ThanhNganKeoTuG(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01, 0.04, 0.2);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_NKTuGiuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MatNganKeo()
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.6, 0.0, 1.0),   // material_ambient (cam)
		color4(1.0, 0.6, 0.0, 1.0),   // material_diffuse (cam)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(0, 0.2, -0.1) * Scale(0.2, 0.1, 0.01);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_NKTuGiuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void DayTu1(GLfloat x)
{
	mat4 instance = Translate(0, x, 0) * Scale(0.2, 0.01, 0.2);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_NKTuGiuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void TayCam1(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.0, 0.5, 1.0),   // material_ambient (tim)
		color4(0.5, 0.0, 0.5, 1.0),   // material_diffuse (tim)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = Translate(x, y, z) * RotateZ(a) * Scale(0.01, 0.05, 0.02);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_NKTuGiuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeoTuGiuong()
{
	DayTu1(0.18);
	MatNganKeo();
	TayCam1(0, 0.22, -0.11, 90);
	ThanhNganKeoTuG(0.08, 0.2, 0);
	ThanhNganKeoTuG(-0.08, 0.2, 0);
}

void canh_tu_dau_giuong()
{

	CanhTu();
	TayCam(-0.05, 0.1, -0.11, 0);
}

void Tu()
{
	model_tuDen = model_room * Translate(xxx + 0.15, yyy + 0.11, zzz - 1.8) * RotateY(bb - 90) * Scale(1.2);
	ThanTu();
	model_NKTuGiuong = model_tuDen * Translate(0, 0, NKTuGiong);
	NganKeoTuGiuong();
	model_CanhTuGiong = model_tuDen * Translate(0.1, 0, -0.1) * RotateY(-aaa) * Translate(-0.1, 0, 0.1);
	canh_tu_dau_giuong();
}


/* ------------------------------------	 Đồng hồ treo tường  ------------------------------------ */

GLfloat  dhx = 0, dhy = 0, dhz = 0, kim = 0;


mat4 model_dong_ho;

void than1_dong_ho(GLfloat x)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(1.0, 0.6, 0.0, 1.0),   // material_ambient (cam)
		color4(1.0, 0.6, 0.0, 1.0),   // material_diffuse (cam)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = RotateZ(x) * Scale(0.2, 0.1, 0.05);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_dong_ho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_dong_ho(GLfloat x)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.0, 0.5, 1.0),   // material_ambient (tim)
		color4(0.5, 0.0, 0.5, 1.0),   // material_diffuse (tim)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = RotateZ(x) * Translate(0, 0.11, 0) * Scale(0.14, 0.02, 0.06);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_dong_ho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void vach_dong_ho(GLfloat x)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.5, 0.0, 0.5, 1.0),   // material_ambient (tim)
		color4(0.5, 0.0, 0.5, 1.0),   // material_diffuse (tim)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = RotateZ(x) * Translate(0.07, 0, -0.03) * Scale(0.015, 0.008, 0.01);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_dong_ho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void kim1_dong_ho(GLfloat x)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.0, 0.0, 0.0, 1.0),   // material_ambient (den)
		color4(0.0, 0.0, 0.0, 1.0),   // material_ambient (den)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = RotateZ(x + 5 * (6 * (kim / 21600))) * Translate(0.03, 0, -0.03) * Scale(0.06, 0.008, 0.01);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_dong_ho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void kim2_dong_ho(GLfloat x)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.0, 0.0, 0.0, 1.0),   // material_ambient (den)
		color4(0.0, 0.0, 0.0, 1.0),   // material_ambient (den)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = RotateZ(x + 6 * (kim / 360)) * Translate(0.035, 0, -0.03) * Scale(0.07, 0.006, 0.006);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_dong_ho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void kim3_dong_ho(GLfloat x)
{
	Phong(
		color4(0.2, 0.2, 0.2, 1.0),   // light_ambient
		color4(1.0, 1.0, 0.8, 1.0),   // light_diffuse
		color4(1.0, 1.0, 1.0, 1.0),   // light_specular
		color4(0.0, 0.0, 0.0, 1.0),   // material_ambient (den)
		color4(0.0, 0.0, 0.0, 1.0),   // material_ambient (den)
		color4(1.0, 1.0, 1.0, 1.0),   // material_specular
		vec4(0.0, 0.0, 1.0, 0.0),     // light_position
		200.0                         // material_shininess
	);
	mat4 instance = RotateZ(x + (kim)) * Translate(0.035, 0, -0.03) * Scale(0.07, 0.004, 0.004);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm11 * model_dong_ho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void than_dong_ho()
{
	than1_dong_ho(0);
	than1_dong_ho(120);
	than1_dong_ho(240);
	canh_dong_ho(60);
	canh_dong_ho(120);
	canh_dong_ho(180);
	canh_dong_ho(-60);
	canh_dong_ho(-120);
	canh_dong_ho(0);
	vach_dong_ho(0);
	vach_dong_ho(90);
	vach_dong_ho(-90);
	vach_dong_ho(180);
}

void dong_ho()
{
	model_dong_ho = model_room * Translate(dhx + 0.012, dhy + 0.6, dhz - 1.85) * RotateY(-90) * Scale(0.7);
	than_dong_ho();
	kim1_dong_ho(60);
	kim2_dong_ho(150);
	kim3_dong_ho(250);
}



/* ------------------------------------	 PHÒNG NGỦ  ------------------------------------ */
void bedroom()
{
	model_room = RotateX(xoay_roomx) * RotateY(xoay_roomy) * RotateZ(xoay_roomz) * Translate(-0.75f, 0.01f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//vẽ tường, sàn của phòng
	build_room();

	//vẽ quạt trần
	quat_tran();

	//vẽ bàn làm việc
	ban_lam_viec();

	//vẽ ghế
	ghe();

	//vẽ tranh
	tranh();

	//vẽ tủ quần áo
	tu_quan_ao();

	//vẽ giường ngủ
	Giuong();

	//vẽ tủ đầu giường
	Tu();

	//vẽ đồng hồ
	dong_ho();
}

float DEGtoRAD(float DEG)
{
	return DEG / 360 * 2 * M_PI;
}


float cam_Rotation[3] = { 0,0,90 };
bool che_do_chi_tiet = false;

// Các tham số cho projection
GLfloat l = -1.0, r = 1.0;
GLfloat bottom = -1.0, top = 1.0;
GLfloat zNear = 1.0, zFar = 4.0;

vec4 eye = vec4(2, 2, -3, 1);
vec4 at = vec4(0, 0, 0, 1);
vec4 up = vec4(0, 1, 0, 1);

//xe tăng
mat4 ctm1 = ctm11;
mat4 instance, instance_tank;

GLfloat theta1[] = { 60, 0, -60,0,0 };
GLfloat n1, m1, o_tank;

void cube(GLfloat w, GLfloat h, GLfloat d)
{
	instance = Scale(w, h, d);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm1 * instance_tank * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void banhxe(GLfloat w, GLfloat h)
{
	for (float i = 0; i < 360; i += 30)
	{
		instance = RotateZ(i) * Scale(w, h, 0.1);
		glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm1 * instance_tank * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void tank()
{
	paintColor(0, 250, 0);
	instance_tank = identity();
	cube(1, 0.2, 0.6);

	instance_tank = Translate(0, 0.1, 0);
	cube(0.9, 0.1, 0.45);

	instance_tank = Translate(0.05, 0.15, 0);
	cube(0.15, 0.2, 0.15);

	instance_tank = Translate(-0.5, 0, 0) * RotateZ(45);
	cube(0.14, 0.14, 0.6);

	instance_tank = Translate(0.5, 0, 0) * RotateZ(45);
	cube(0.14, 0.14, 0.6);

	instance_tank = Translate(0, 0.15, -0.3);
	cube(0.9, 0.01, 0.15);
	instance_tank = Translate(0, 0.14, -0.4) * RotateX(-30);
	cube(0.9, 0.01, 0.08);
	instance_tank = Translate(0, 0.15, 0.3);
	cube(0.9, 0.01, 0.15);
	instance_tank = Translate(0, 0.14, 0.4) * RotateX(30);
	cube(0.9, 0.01, 0.08);

	instance_tank = Translate(0.52, 0.12, -0.3) * RotateZ(-30);
	cube(0.16, 0.01, 0.15);
	instance_tank = Translate(0.52, 0.12, 0.3) * RotateZ(-30);
	cube(0.16, 0.01, 0.15);
	instance_tank = Translate(-0.52, 0.12, -0.3) * RotateZ(30);
	cube(0.16, 0.01, 0.15);
	instance_tank = Translate(-0.52, 0.12, 0.3) * RotateZ(30);
	cube(0.16, 0.01, 0.15);

	instance_tank = Translate(0, 0.15, -0.275);
	cube(1, 0.1, 0.25);
	instance_tank = Translate(0, 0.15, 0.275);
	cube(1, 0.1, 0.25);
	paintColor(00, 00, 00);
	instance_tank = Translate(-0.4f, -0.05f, 0.3f) * RotateZ(theta1[4]);
	banhxe(0.1f, 0.2f);
	instance_tank = Translate(-0.15f, -0.1f, 0.3f) * RotateZ(theta1[4]);
	banhxe(0.2f, 0.2f);
	instance_tank = Translate(0.15f, -0.1f, 0.3f) * RotateZ(theta1[4]);
	banhxe(0.2f, 0.2f);
	instance_tank = Translate(0.4f, -0.05f, 0.3f) * RotateZ(theta1[4]);
	banhxe(0.1f, 0.2f);

	instance_tank = Translate(-0.4f, -0.05f, -0.3f) * RotateZ(theta1[4]);
	banhxe(0.1f, 0.2f);
	instance_tank = Translate(-0.15f, -0.1f, -0.3f) * RotateZ(theta1[4]);
	banhxe(0.2f, 0.2f);
	instance_tank = Translate(0.15f, -0.1f, -0.3f) * RotateZ(theta1[4]);
	banhxe(0.2f, 0.2f);
	instance_tank = Translate(0.4f, -0.05f, -0.3f) * RotateZ(theta1[4]);
	banhxe(0.1f, 0.2f);
}
void phao(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, l, h);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm1 * instance_tank * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void phao2(GLfloat w, GLfloat l, GLfloat h) {
	instance = Scale(w, h, l) * Translate(0.0, 0.45, 0.0);
	glUniformMatrix4fv(model_loc, GL_TRUE, 1, ctm1 * instance_tank * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void bephao() {

	paintColor(0, 255, 0);
	instance_tank = Translate(0.0f, 0.3f, 0.0f);
	phao(0.7f, 0.15f, 0.7f);
	paintColor(0, 250, 100);
	instance_tank = Translate(0.275f, 0.45f, 0.0f);
	phao(0.15f, 0.15f, 0.15f);
	paintColor(100, 250, 100);
	instance_tank = Translate(0.275f, 0.45f, 0.0f) * RotateZ(theta1[2]);
	phao2(0.07f, 0.07f, 0.6);
	paintColor(200, 250, 200);
	instance_tank = Translate(0.275f, 0.45f, 0.0f) * RotateZ(theta1[2]) * Translate(0.0f, n, 0.0f);
	phao2(0.06f, 0.06f, 0.1);
	paintColor(0, 250, 100);
	instance_tank = Translate(0.275f, 0.55f, 0.0f);
	phao(0.1f, 0.1f, 0.1f);
	instance_tank = Translate(-0.1f, 0.7f, 0.0f);
	phao(0.1f, 0.1f, 0.1f);
	instance_tank = Translate(-0.1f, 0.6f, 0.0f);
	phao(0.05f, 0.1f, 0.01f);
	instance_tank = Translate(-0.1f, 0.5f, 0.0f);
	phao(0.15f, 0.1f, 0.15f);
	instance_tank = Translate(0.0f, 0.4f, 0.0f);
	phao(0.45f, 0.15f, 0.45f);
	instance_tank = Translate(-0.2f, 0.5f, 0.0f);
	phao(0.01f, 0.15f, 0.2f);
}
void banphao(void)
{
	n1 += 0.03;
	glutPostRedisplay();
}
bool xeTangMau;
void xetang(GLfloat w, GLfloat e, GLfloat r, bool xeMau)
{
	if (xeMau) {
		ctm1 = Translate(-0.13, -0.48, -0.1) * RotateY(-30) * Scale(0.3, 0.3, 0.3);
		ctm1 = ctm1 * Translate(m, 0.0f, o_tank) * RotateY(theta1[1]);
		tank();
		ctm1 = ctm1 * RotateY(theta1[3]);
		bephao();
	}
	else {
		ctm1 = Translate(w, e, r) * Scale(0.3, 0.3, 0.3);
		ctm1 = ctm1 * Translate(m, 0.0f, o_tank) * RotateY(theta1[1]);
		tank();
		ctm1 = ctm1 * RotateY(theta1[3]);
		bephao();
	}



}
void btl() {
	ghe1();
	banthungan();
	bantrungbay();
	xetai();
	cancau();
	robot();
	be_phong();
	tu();
	tu2();
	fan_display();
	dong_ho1();
	//tuong();
}
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ctm11 = Scale(6) * Translate(0.0f, 0.49f, -0.48f)*RotateY(-90);
	bedroom();
	ctm1 = Scale(10);
	xetang(0, 0.1, -1, xeTangMau);
	btl();
	
	const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/
	glutSwapBuffers();

}
float cam_x = 0, cam_y = 0;
void reshape(int width, int height)
{
	vec4 eye(0, 1, 4, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 3, 0, 3);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-2, 2, -2, 2, 1, 100);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glViewport(0, 0, width, height);
}
// r đó kiểu mà để chỗ vẽ tầng 2 í hoặc góc nhìn như này được k

void update_camera()
{
	vec4 eye(0, 1 + cam_y, 4 + cam_x, 1);
	vec4 at(0, 0 + cam_y, 0 + cam_x, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-2, 2, -2, 2, 1, 100);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler
	//cout << "key " << key << endl;
	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'a':
		Theta[0] += 5;
		glutPostRedisplay();
		break;
	case 's':
		Theta[1] += 5;
		glutPostRedisplay();
		break;
	case 'd':
		Theta[4] += 5;
		glutPostRedisplay();
		break;
	case 'f':
		Theta[5] -= 5;
		glutPostRedisplay();
		break;
	case '2':
		Theta[6] -= 5;
		glutPostRedisplay();
		break;
	case 'h':
		if (i < 0.02)
			i += 0.01;
		glutPostRedisplay();
		break;
	case 'H':
		if (i > 0)
			i -= 0.01;
		glutPostRedisplay();
		break;
	case 'g':
		if ((Theta[3] >= -90)) {
			Theta[3] -= 5;
		}
		glutPostRedisplay();
		break;
	case 'G':
		if ((Theta[3] < 0)) {
			Theta[3] += 5;
		}
		glutPostRedisplay();
		break;
	case 'j':
		if (m < 0.5)
			m += 0.01;
		glutPostRedisplay();
		break;
	case 'J':
		if (m > 0)
			m -= 0.01;
		glutPostRedisplay();
		break;
		/*---------------------- -*/
	case 'z':
		theta[0] += 5;
		if (theta[0] > 360) theta[0] -= 360;
		glutPostRedisplay();
		break;
	case 'x':
		theta[1] += 5;
		if (theta[1] > 360) theta[1] -= 360;
		glutPostRedisplay();
		break;
	case 'c':
		glutIdleFunc(spinArm);
		break;
	case 'v':
		theta[3] += 5;
		if (theta[3] > 150) theta[3] -= 150;
		glutPostRedisplay();
		break;
	case 'b':
		theta[5] += 5;
		if (theta[5] > 150) theta[5] -= 150;
		glutPostRedisplay();
		break;
	case 'n':
		theta[5] += 5;
		if (theta[5] > 150) theta[5] -= 150;
		glutPostRedisplay();
		break;
	case 'q':
		glutIdleFunc(sit);
		break;
	case 'N':
		glutIdleFunc(unsit);
		break;
	case 'm':
		theta[7] += 5;
		if (theta[7] > 60) theta[7] -= 60;
		glutPostRedisplay();
		break;
	case 'M':
		theta[9] += 5;
		if (theta[9] > 60) theta[9] -= 60;
		glutPostRedisplay();
		break;
		/*--------------------TỦ--------------*/
	case '1':
		Theta[2] += 5;
		glutPostRedisplay();
		break;
		/*--------TÊN LỬA------------*/
	// Quay đế
	case 'e':
		alpha += 10;
		glutPostRedisplay();
		break;
	case 'r':
		alpha -= 10;
		glutPostRedisplay();
		break;
		//nâng giá tên lửa
	case 't':
		beta += 10;
		if (beta >= 60) beta = 60;
		glutPostRedisplay();
		break;
	case 'T':
		beta -= 10;
		if (beta <= 0) beta = 0;
		glutPostRedisplay();
		break;
		//Phóng tên
	case 'y':
		yy += cao_than;
		gama -= 5;
		if (gama * -1 > beta + 10) {
			yy = 0;
			gama = 0;
		}
		glutPostRedisplay();
		break;
	case '9':
		isRorate = !isRorate;
		glutPostRedisplay();
		break;

	case '4':
		dong_ho_x += 0.1;
		glutPostRedisplay();
		break;
	case '5':
		dong_ho_x -= 0.1;
		glutPostRedisplay();
		break;
	case '6':
		dong_ho_y += 0.1;
		glutPostRedisplay();
		break;
	case '7':
		dong_ho_y -= 0.1;
		glutPostRedisplay();
		break;


	case '/':
		cam_x += 0.5;
		glutPostRedisplay();
		break;
	case '*':
		cam_x -= 0.5;
		glutPostRedisplay();
		break;
	case '-':
		cam_y += 0.5;
		glutPostRedisplay();
		break;
	case '+':
		cam_y -= 0.5;
		glutPostRedisplay();
		break;
	
	
	

		//công tắc quạt
	case 'B':
		checked = !checked;   // thay đổi giá trị true - false
		glutPostRedisplay();
		break;

	case 'C':
		xoay_quaty += 5;
		glutPostRedisplay();
		break;

		//điều khiển bàn
	case 'i':
		di_chuyen_banx -= 0.05f;
		if (di_chuyen_banx < -0.1f) di_chuyen_banx = -0.1f;
		glutPostRedisplay();
		break;

	case 'I':
		di_chuyen_banx += 0.05f;
		if (di_chuyen_banx > 0.0f) di_chuyen_banx = 0.0f;
		glutPostRedisplay();
		break;

	case 'k':
		if (di_chuyenx < -0.4f) {
			if (xx < 0.2f) {
				if (di_chuyen_demZ >= -0.2f) {
					di_chuyen_banz -= 0.1f;
					if (di_chuyen_banz < -1.6f) di_chuyen_banz = -1.6f;
				}
				else {
					di_chuyen_banz -= 0.1f;
					if (di_chuyen_banz < -0.4f) di_chuyen_banz = -0.4f;
				}
			}
			else {

				di_chuyen_banz -= 0.1f;
				if (di_chuyen_banz < -0.4f) di_chuyen_banz = -0.4f;
			}
		}
		else {
			di_chuyen_banz = 0.0f;
		}
		glutPostRedisplay();
		break;

	case 'K':
		if (di_chuyenx < -0.4f) {
			if (xx < 0.2f) {
				if (di_chuyen_demZ >= -0.2f) {
					if (di_chuyen_banz < -1.6f) {
						di_chuyen_banz = -1.6f;
					}
					else {
						di_chuyen_banz += 0.1f;
						if (di_chuyen_banz > 0.0f) di_chuyen_banz = 0.0f;
					}
				}
			}
			else {
				if (di_chuyen_banz >= -0.4f) {
					di_chuyen_banz += 0.1f;
					if (di_chuyen_banz > 0.0f) di_chuyen_banz = 0.0f;
				}
				else {
					di_chuyen_banz = -1.6f;
				}
			}
		}
		else {
			di_chuyen_banz = 0.0f;
		}

		glutPostRedisplay();
		break;

		//cánh tủ bàn
	case 'l': //mở cánh tủ
		if (di_chuyenx < 0.0f && di_chuyenx > -0.5f) {
			mo_canh_tu_ban = 0;
		}
		else if (di_chuyenz <= -0.2f) {
			mo_canh_tu_ban += 5;
			if (mo_canh_tu_ban > 120) mo_canh_tu_ban = 120;
		}
		else {
			mo_canh_tu_ban += 5;
			if (mo_canh_tu_ban > 120) mo_canh_tu_ban = 120;
		}
		glutPostRedisplay();
		break;

	case 'L': //đóng cánh tủ
		mo_canh_tu_ban -= 5;
		if (mo_canh_tu_ban < 0) mo_canh_tu_ban = 0;
		glutPostRedisplay();
		break;

		//ngăn kéo bàn
	case 'o': //kéo ngăn kéo ra
		if (di_chuyenx < 0.0f && di_chuyenx > -0.5f) {
			if (di_chuyenz <= -0.2f) {
				keo_ngan_keo_banz -= 0.1f;
				if (keo_ngan_keo_banz < -0.2f) keo_ngan_keo_banz = -0.2f;
			}
			else {
				keo_ngan_keo_banz = 0;
			}
		}
		else {
			keo_ngan_keo_banz -= 0.1f;
			if (keo_ngan_keo_banz < -0.2f) keo_ngan_keo_banz = -0.2f;
		}

		glutPostRedisplay();
		break;

	case 'O': //đẩy ngăn kéo vào
		keo_ngan_keo_banz += 0.1f;
		if (keo_ngan_keo_banz > 0) keo_ngan_keo_banz = 0;
		glutPostRedisplay();
		break;

		//di chuyển ghế
	case 'p': // kéo ghế ra
		di_chuyenz -= 0.1f;
		if (xx > 0.4f && di_chuyenz < -0.2f) {
			di_chuyenz = -0.2f;
		}
		else {
			if (di_chuyenz < -1.2f) di_chuyenz = -1.2f;
		}
		glutPostRedisplay();
		break;

	case 'P': // đẩy ghế vào
		if (xx > 0.4f && di_chuyenz <= -1.2f) {
			di_chuyenz = -1.2f;
		}
		else if (xx > 0.4f && di_chuyenz >= -0.2f) {
			di_chuyenz += 0.1f;
			if (di_chuyenz > 0.0f) di_chuyenz = 0.0f;
		}
		else {
			di_chuyenz += 0.1f;
			if (di_chuyenz > 0.0f) di_chuyenz = 0.0f;
		}
		glutPostRedisplay();
		break;

	case 'w': //kéo ghế sang phải
		if (keo_ngan_keo_banz < 0.0f && di_chuyenx == 0.0f) {
			di_chuyenx = 0.0f;
		}
		else {
			di_chuyenx -= 0.1f;
			if (zz >= 0.1f && di_chuyenx < -0.4f) {
				di_chuyenx = -0.4f;
			}
			else if (di_chuyenz < -0.3f && di_chuyenx < -(0.4f - xx)) {
				di_chuyenx = -(0.4f - xx);
			}
			else if (di_chuyenx < -1.0f) {
				di_chuyenx = -1.0f;
			}
		}
		glutPostRedisplay();
		break;
	case 'W': //kéo ghế sang trái
		if (keo_ngan_keo_banz < 0.0f) {
			di_chuyenx += 0.1f;
			if (di_chuyenx > -0.5f) di_chuyenx = -0.5f;
		}
		else if (mo_canh_tu_ban > 5) {
			di_chuyenx += 0.1f;
			if (di_chuyenx > -0.5f) di_chuyenx = -0.5f;
		}
		else if (di_chuyen_banz <= -0.1f) {
			di_chuyenx += 0.1f;
			if (di_chuyenx > -0.5f) di_chuyenx = -0.5f;
		}
		else {
			di_chuyenx += 0.1f;
			if (di_chuyenx > 0.1f) di_chuyenx = 0.1f;
		}
		glutPostRedisplay();
		break;

		//quay ghế
	case '.':
		quay_ghe += 5;
		glutPostRedisplay();
		break;

	case '>':
		quay_ghe -= 5;
		glutPostRedisplay();
		break;

		// di chuyển tranh
	case ',':
		di_chuyen_tranhz -= 0.1f;
		if (di_chuyen_tranhz < -0.2f) di_chuyen_tranhz = -0.2f;
		glutPostRedisplay();
		break;

	case '<':
		di_chuyen_tranhz += 0.1f;
		if (di_chuyen_tranhz > 0.4f) di_chuyen_tranhz = 0.4f;
		glutPostRedisplay();
		break;

		//di chuyển tủ quần áo
	case '{':
		di_chuyen_tu_quan_aox += 0.1f;
		if (di_chuyen_tu_quan_aox > 0.1f) di_chuyen_tu_quan_aox = 0.1f;
		glutPostRedisplay();
		break;

	case '}':
		di_chuyen_tu_quan_aox -= 0.1f;
		if (di_chuyen_tu_quan_aox < 0.0f) di_chuyen_tu_quan_aox = 0.0f;
		glutPostRedisplay();
		break;

		//điều khiển cánh trái tủ quần áo
	case '[': //mở
		quay_canh_phai_tu_quan_ao -= 5;
		if (quay_canh_phai_tu_quan_ao < -90) quay_canh_phai_tu_quan_ao = -90;
		glutPostRedisplay();
		break;
	case ']': //đóng
		quay_canh_phai_tu_quan_ao += 5;
		if (quay_canh_phai_tu_quan_ao > 0) quay_canh_phai_tu_quan_ao = 0;
		glutPostRedisplay();
		break;

		//điều khiển cánh phải tủ quần áo
	case 'u':
		quay_canh_trai_tu_quan_ao += 5;
		if (quay_canh_trai_tu_quan_ao > 90) quay_canh_trai_tu_quan_ao = 90;
		glutPostRedisplay();
		break;
	case 'U':
		quay_canh_trai_tu_quan_ao -= 5;
		if (quay_canh_trai_tu_quan_ao < 0) quay_canh_trai_tu_quan_ao = 0;
		glutPostRedisplay();
		break;

		//điều khiển ngắn kéo tủ quần áo
	case '0': //kéo ra
		di_chuyen_ngan_keo_tu_quan_aoz -= 0.1f;
		if (di_chuyen_ngan_keo_tu_quan_aoz < -0.2f) di_chuyen_ngan_keo_tu_quan_aoz = -0.2f;
		if (di_chuyenx < -0.5f) di_chuyen_ngan_keo_tu_quan_aoz = 0.0f;
		glutPostRedisplay();
		break;

	case ')': //đẩy vào
		di_chuyen_ngan_keo_tu_quan_aoz += 0.1f;
		if (di_chuyen_ngan_keo_tu_quan_aoz > 0.0f) di_chuyen_ngan_keo_tu_quan_aoz = 0.0f;
		glutPostRedisplay();
		break;

		//điều khiển giường
	case '#'://quay đệm Y
		if (di_chuyen_demZ < -0.2) xoay_demY += 5;
		glutPostRedisplay();
		break;
	case '$':
		if (di_chuyen_demZ < -0.2) xoay_demY -= 5;
		glutPostRedisplay();
		break;
	case '%'://quay đệm Z
		if (di_chuyen_demZ < -0.4 && di_chuyen_demY > -0.27)
		{
			if (xoay_demX < -35 + di_chuyen_demZ * xoay_demX + 5) xoay_demX = -35 + di_chuyen_demZ * xoay_demX, goi1 = -0.5, goi2 = -0.5;
			{
				xoay_demX -= 5;
				if (xoay_demX < -10)
				{
					goi1 -= 0.17, goi2 -= 0.17;
					if (goi1 < -0.5) goi1 = -0.5;
					if (goi2 < -0.5) goi2 = -0.5;
				}
			}
		}

		glutPostRedisplay();
		break;

	case '^':
		if (di_chuyen_demZ < -0.4)
		{
			xoay_demX += 5;
			if (xoay_demX > 0) xoay_demX = 0;
		}
		glutPostRedisplay();
		break;
	case 'X'://di chuyển đệm
		di_chuyen_demZ += 0.05, di_chuyen_demY = 0;
		if (di_chuyen_demZ > 0) di_chuyen_demZ = 0;
		if (di_chuyen_demZ > -0.4) xoay_demX = 0;
		if (di_chuyen_demZ > -0.2) xoay_demY = 0;
		glutPostRedisplay();
		break;
	case ';':
		di_chuyen_demZ -= 0.05;
		if (di_chuyen_demZ < -0.8) di_chuyen_demZ = -0.8, di_chuyen_demY = -0.27, xoay_demX = 0;
		glutPostRedisplay();
		break;
	case ':'://di chuyển gối
		goi1 += 0.05;
		if (goi1 > 0) goi1 = 0;
		glutPostRedisplay();
		break;
	case '`':
		goi1 -= 0.05;
		if (goi1 < -0.5) goi1 = -0.5;
		glutPostRedisplay();
		break;
	case '~'://di chuyển gối
		goi2 += 0.05;
		if (goi2 > 0.05) goi2 = 0;
		glutPostRedisplay();
		break;
	case '3':
		goi2 -= 0.05;
		if (goi2 < -0.5) goi2 = -0.5;
		glutPostRedisplay();
		break;

	case 'Q'://di chuyển giường dọc
		xx -= 0.1;
		if (xx < 0) xx = 0;
		glutPostRedisplay();
		break;
	case 'S':
		if (di_chuyenz <= -1.2f) {
			xx += 0.1f;
			if (xx > 0.7f) xx = 0.7f;
		}
		else if (di_chuyenz < -0.2f && xx >(0.3f + di_chuyenx)) {
			xx = (0.4f + di_chuyenx);
		}
		else {
			xx += 0.1f;
			if (xx > 0.7f) xx = 0.7f;
		}
		glutPostRedisplay();
		break;

	case 'E'://di chuyển giường ngang
		zz -= 0.1;
		if (zz < 0) zz = 0;
		glutPostRedisplay();
		break;
	case 'R':
		zz += 0.1;
		if (zz > 0.3) zz = 0.3;
		glutPostRedisplay();
		break;

	case '8'://quay Giường
		aa -= 10;
		glutPostRedisplay();
		break;
	case 'Y':
		aa += 10;
		glutPostRedisplay();
		break;

		// di chuyển tủ đầu giường
	case '!'://quay cánh
		aaa -= 10;
		if (aaa < 0) aaa = 0;
		glutPostRedisplay();
		break;
	case '@':
		aaa += 10;
		if (aaa > 110) aaa = 110;
		glutPostRedisplay();
		break;
	case 'D'://di chuyển ngan keo
		NKTuGiong -= 0.01;
		if (NKTuGiong < -0.15) NKTuGiong = -0.15;
		glutPostRedisplay();
		break;
	case 'F':
		NKTuGiong += 0.01;
		if (NKTuGiong > 0) NKTuGiong = 0;
		glutPostRedisplay();
		break;

	case 'Z'://quay tủ
		bb -= 10;
		glutPostRedisplay();
		break;
	case 'V':
		bb += 10;
		glutPostRedisplay();
		break;

	case '&'://di chuyển ngang
		xxx -= 0.01;
		if (xxx < 0) xxx = 0;
		glutPostRedisplay();
		break;
	case '(':
		xxx += 0.01;
		if (xxx > 1.1) xxx = 1.1;
		glutPostRedisplay();
		break;

	case ' '://di chuyển dọc
		zzz -= 0.01;
		if (zzz < -0.1) zzz = -0.1;
		glutPostRedisplay();
		break;
	case '_':
		zzz += 0.01;
		if (zzz > 0.8) zzz = 0.8;
		glutPostRedisplay();
		break;
	case '|':
		kim += 90;
		glutPostRedisplay();
		break;
		
	default:
		break;
	}

	update_camera();

}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");
	glewInit();
	generateGeometry();
	initGPUBuffers();
	shaderSetup();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
