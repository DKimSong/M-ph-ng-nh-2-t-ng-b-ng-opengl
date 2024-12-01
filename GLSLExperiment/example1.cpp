//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


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
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuy?n ?ng v?i m?i d?nh*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program, model, projection, view;


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
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
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

	model = glGetUniformLocation(program, "model");
	projection = glGetUniformLocation(program, "projection");
	view = glGetUniformLocation(program, "view");

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
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


/* ------------------------------------ DỰNG HÌNH CĂN PHÒNG  ------------------------------------ */
mat4 model_room, ctm;
GLfloat xoay_roomx = 0, xoay_roomy = 0, xoay_roomz = 0;

void san()
{
	mat4 instance = Translate(0.75f, 0.01f, -1.0f) * Scale(1.5f, 0.02f, 2.0f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong_sau()
{
	mat4 instance = Translate(0.75f, 0.4f, -0.01f) * Scale(1.5f, 0.8f, 0.02f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong_trai()
{
	mat4 instance = Translate(-0.01f, 0.4f, -1.0f) * Scale(0.02f, 0.8f, 2.0f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_room * instance);
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
}

/* ------------------------------------ QUẠT TRẦN ------------------------------------ */
mat4 model_quat;
GLfloat xoay_quaty = 0; //điều khiển quạt

void truc_dai()
{
	mat4 instance = Scale(0.01f, 0.1f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_quat * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void truc_quay()
{
	mat4 instance = Translate(0.0f, -0.065f, 0.0f) * Scale(0.01f, 0.01f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_quat * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_quat(GLfloat x)
{
	mat4 instance = RotateY(x) * Translate(0.08f, -0.065f, 0.0f) * Scale(0.15f, 0.01f, 0.03f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_quat * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chan_ban(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.2f, 0.3f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_day(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.2f, 0.01f, 0.3f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_canh_tu_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_canh_tu_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ban * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ghe * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ghe * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ghe * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ghe * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ghe * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tranh * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tranh * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chan_tu(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.6f, 0.3f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void mat_tren(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.8f, 0.01f, 0.3f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_treo(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.79f, 0.01f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_doc(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.05f, 0.6f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_ngang(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.7f, 0.03f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tu_quan_ao * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * modelx * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tay_cam_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z, mat4 modelx)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.04f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * modelx * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_trai()
{
	canh_tu_quan_ao(0.245f, 0.39f, -0.36f, model_canh_trai);
	tay_cam_tu_quan_ao(0.4f, 0.39f, -0.365f, model_canh_trai);
}

void canh_phai()
{
	canh_tu_quan_ao(0.595f, 0.39f, -0.36f, model_canh_phai);
	tay_cam_tu_quan_ao(0.44f, 0.39f, -0.365f, model_canh_phai);
}

// vẽ ngăn kéo tủ quần áo
mat4 model_ngan_keo_tu_quan_ao;

GLfloat di_chuyen_ngan_keo_tu_quan_aoz = 0;

void thanh_ngang_ngan_keo(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.7f, 0.185f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanh_doc_ngan_keo(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.01f, 0.185f, 0.24f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void day_ngan_keo_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.68f, 0.01f, 0.23f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo_tu_quan_ao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tay_cam_ngan_keo_tu_quan_ao(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.04f, 0.01f, 0.01f);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_ngan_keo_tu_quan_ao * instance);
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
GLfloat  zz = 0, yy = 0, xx = 0, aa = 0, di_chuyen_demX = 0, di_chuyen_demZ = 0, di_chuyen_demY = 0;

void ThanGiuong()
{
	mat4 instance = Translate(0, 0, 0) * Scale(0.8, 0.2, 0.8);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_giuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ChanGiuong(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.05, 0.25, 0.05);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_giuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ThanhGiuong(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.8, 0.2, 0.05);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_giuong * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_giuong * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_goi1 * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_goi2 * instance);
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
	model_giuong = model_room * Translate(xx + 0.4, yy + 0.2, zz - 1.2) * Scale(0.9) * RotateY(aa - 90);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tuDen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void ThanhTu(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	mat4 instance = Translate(x, y, z) * RotateY(a) * Scale(0.2, 0.3, 0.01);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tuDen * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_CanhTuGiong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ChanTu(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(x, y, z) * Scale(0.02, 0.3, 0.02);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_tuDen * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_CanhTuGiong * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_NKTuGiuong * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_NKTuGiuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void DayTu1(GLfloat x)
{
	mat4 instance = Translate(0, x, 0) * Scale(0.2, 0.01, 0.2);
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_NKTuGiuong * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_NKTuGiuong * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_dong_ho * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_dong_ho * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_dong_ho * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_dong_ho * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_dong_ho * instance);
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
	glUniformMatrix4fv(model, GL_TRUE, 1, ctm * model_dong_ho * instance);
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


void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ctm = Scale(1.5);

	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(view, 1, GL_TRUE, mv);

	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	bedroom();
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}


void keyboard(unsigned char key, int x, int y)
{
	vec4 cam_forward = normalize(eye - at);
	vec4 cam_right = normalize(cross(cam_forward, up));
	vec4 cam_up = normalize(cross(cam_right, cam_forward));

	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;

	
	
	case 't':
		che_do_chi_tiet = !che_do_chi_tiet;
		glutPostRedisplay();
		break;

	case 'w':
		eye = eye - cam_forward * (che_do_chi_tiet ? 0.1 : 1);
		glutPostRedisplay();
		break;
	case 'W':
		eye = eye + cam_forward * (che_do_chi_tiet ? 0.1 : 1);
		glutPostRedisplay();
		break;
	case 'e':
		eye = eye - cam_right * (che_do_chi_tiet ? 0.1 : 1);
		glutPostRedisplay();
		break;
	case 'E':
		eye = eye + cam_right * (che_do_chi_tiet ? 0.1 : 1);
		glutPostRedisplay();
		break;
	case 'r':
		eye = eye - cam_up * (che_do_chi_tiet ? 0.1 : 1);
		glutPostRedisplay();
		break;
	case 'R':
		eye = eye + cam_up * (che_do_chi_tiet ? 0.1 : 1);
		glutPostRedisplay();
		break;

		//công tắc quạt
	case 'q':
		checked = !checked;   // thay đổi giá trị true - false
		glutPostRedisplay();
		break;

	case 'Q':
		xoay_quaty += 5;
		glutPostRedisplay();
		break;

		//điều khiển bàn
	case 'j':
		di_chuyen_banx -= 0.05f;
		if (di_chuyen_banx < -0.1f) di_chuyen_banx = -0.1f;
		glutPostRedisplay();
		break;

	case 'J':
		di_chuyen_banx += 0.05f;
		if (di_chuyen_banx > 0.0f) di_chuyen_banx = 0.0f;
		glutPostRedisplay();
		break;

	case '`':
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

	case '~':
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
	case 'm': //mở cánh tủ
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

	case 'M': //đóng cánh tủ
		mo_canh_tu_ban -= 5;
		if (mo_canh_tu_ban < 0) mo_canh_tu_ban = 0;
		glutPostRedisplay();
		break;

		//ngăn kéo bàn
	case 'n': //kéo ngăn kéo ra
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

	case 'N': //đẩy ngăn kéo vào
		keo_ngan_keo_banz += 0.1f;
		if (keo_ngan_keo_banz > 0) keo_ngan_keo_banz = 0;
		glutPostRedisplay();
		break;

		//di chuyển ghế
	case 'g': // kéo ghế ra
		di_chuyenz -= 0.1f;
		if (xx > 0.4f && di_chuyenz < -0.2f) {
			di_chuyenz = -0.2f;
		}
		else {
			if (di_chuyenz < -1.2f) di_chuyenz = -1.2f;
		}
		glutPostRedisplay();
		break;

	case 'G': // đẩy ghế vào
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

	case 'f': //kéo ghế sang phải
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
	case 'F': //kéo ghế sang trái
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
	case 'h':
		quay_ghe += 5;
		glutPostRedisplay();
		break;

	case 'H':
		quay_ghe -= 5;
		glutPostRedisplay();
		break;

		// di chuyển tranh
	case '.':
		di_chuyen_tranhz -= 0.1f;
		if (di_chuyen_tranhz < -0.2f) di_chuyen_tranhz = -0.2f;
		glutPostRedisplay();
		break;

	case '>':
		di_chuyen_tranhz += 0.1f;
		if (di_chuyen_tranhz > 0.4f) di_chuyen_tranhz = 0.4f;
		glutPostRedisplay();
		break;

		//di chuyển tủ quần áo
	case 'a':
		di_chuyen_tu_quan_aox += 0.1f;
		if (di_chuyen_tu_quan_aox > 0.1f) di_chuyen_tu_quan_aox = 0.1f;
		glutPostRedisplay();
		break;

	case 'A':
		di_chuyen_tu_quan_aox -= 0.1f;
		if (di_chuyen_tu_quan_aox < 0.0f) di_chuyen_tu_quan_aox = 0.0f;
		glutPostRedisplay();
		break;

		//điều khiển cánh trái tủ quần áo
	case '1': //mở
		quay_canh_phai_tu_quan_ao -= 5;
		if (quay_canh_phai_tu_quan_ao < -90) quay_canh_phai_tu_quan_ao = -90;
		glutPostRedisplay();
		break;
	case '2': //đóng
		quay_canh_phai_tu_quan_ao += 5;
		if (quay_canh_phai_tu_quan_ao > 0) quay_canh_phai_tu_quan_ao = 0;
		glutPostRedisplay();
		break;

		//điều khiển cánh phải tủ quần áo
	case '3':
		quay_canh_trai_tu_quan_ao += 5;
		if (quay_canh_trai_tu_quan_ao > 90) quay_canh_trai_tu_quan_ao = 90;
		glutPostRedisplay();
		break;
	case '4':
		quay_canh_trai_tu_quan_ao -= 5;
		if (quay_canh_trai_tu_quan_ao < 0) quay_canh_trai_tu_quan_ao = 0;
		glutPostRedisplay();
		break;

		//điều khiển ngắn kéo tủ quần áo
	case '5': //kéo ra
		di_chuyen_ngan_keo_tu_quan_aoz -= 0.1f;
		if (di_chuyen_ngan_keo_tu_quan_aoz < -0.2f) di_chuyen_ngan_keo_tu_quan_aoz = -0.2f;
		if (di_chuyenx < -0.5f) di_chuyen_ngan_keo_tu_quan_aoz = 0.0f;
		glutPostRedisplay();
		break;

	case '6': //đẩy vào
		di_chuyen_ngan_keo_tu_quan_aoz += 0.1f;
		if (di_chuyen_ngan_keo_tu_quan_aoz > 0.0f) di_chuyen_ngan_keo_tu_quan_aoz = 0.0f;
		glutPostRedisplay();
		break;

		//điều khiển giường
	case 'p'://quay đệm Y
		if (di_chuyen_demZ < -0.2) xoay_demY += 5;
		glutPostRedisplay();
		break;
	case 'P':
		if (di_chuyen_demZ < -0.2) xoay_demY -= 5;
		glutPostRedisplay();
		break;
	case '['://quay đệm Z
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

	case ']':
		if (di_chuyen_demZ < -0.4)
		{
			xoay_demX += 5;
			if (xoay_demX > 0) xoay_demX = 0;
		}
		glutPostRedisplay();
		break;
	case ':'://di chuyển đệm
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
	case 'O'://di chuyển gối
		goi1 += 0.05;
		if (goi1 > 0) goi1 = 0;
		glutPostRedisplay();
		break;
	case 'o':
		goi1 -= 0.05;
		if (goi1 < -0.5) goi1 = -0.5;
		glutPostRedisplay();
		break;
	case ')'://di chuyển gối
		goi2 += 0.05;
		if (goi2 > 0.05) goi2 = 0;
		glutPostRedisplay();
		break;
	case '0':
		goi2 -= 0.05;
		if (goi2 < -0.5) goi2 = -0.5;
		glutPostRedisplay();
		break;


	case 'I'://di chuyển giường dọc
		xx -= 0.1;
		if (xx < 0) xx = 0;
		glutPostRedisplay();
		break;
	case 'i':
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

	case 'U'://di chuyển giường ngang
		zz -= 0.1;
		if (zz < 0) zz = 0;
		glutPostRedisplay();
		break;
	case 'u':
		zz += 0.1;
		if (zz > 0.3) zz = 0.3;
		glutPostRedisplay();
		break;

	case '@'://quay Giường
		aa -= 10;
		glutPostRedisplay();
		break;
	case '#':
		aa += 10;
		glutPostRedisplay();
		break;

		// di chuyển tủ đầu giường
	case 'K'://quay cánh
		aaa -= 10;
		if (aaa < 0) aaa = 0;
		glutPostRedisplay();
		break;
	case 'k':
		aaa += 10;
		if (aaa > 110) aaa = 110;
		glutPostRedisplay();
		break;
	case '/'://di chuyển ngan keo
		NKTuGiong -= 0.01;
		if (NKTuGiong < -0.15) NKTuGiong = -0.15;
		glutPostRedisplay();
		break;
	case '?':
		NKTuGiong += 0.01;
		if (NKTuGiong > 0) NKTuGiong = 0;
		glutPostRedisplay();
		break;

	case 'c'://quay tủ
		bb -= 10;
		glutPostRedisplay();
		break;
	case 'C':
		bb += 10;
		glutPostRedisplay();
		break;

	case 'B'://di chuyển ngang
		xxx -= 0.01;
		if (xxx < 0) xxx = 0;
		glutPostRedisplay();
		break;
	case 'b':
		xxx += 0.01;
		if (xxx > 1.1) xxx = 1.1;
		glutPostRedisplay();
		break;

	case 'v'://di chuyển dọc
		zzz -= 0.01;
		if (zzz < -0.1) zzz = -0.1;
		glutPostRedisplay();
		break;
	case 'V':
		zzz += 0.01;
		if (zzz > 0.8) zzz = 0.8;
		glutPostRedisplay();
		break;
	case '$':
		kim += 90;
		glutPostRedisplay();
		break;



		//điều khiển phóng to thu nhỏ (l, r, top ,bottom, zNear, zFar :biến khởi tạo camera)
	case 'x':
		l *= 1.1f; r *= 1.1f;
		glutPostRedisplay();
		break;
	case 'X':
		l *= 0.9f; r *= 0.9f;
		glutPostRedisplay();
		break;
	case 'y':
		bottom *= 1.1f; top *= 1.1f;
		glutPostRedisplay();
		break;
	case 'Y':
		bottom *= 0.9f; top *= 0.9f;
		glutPostRedisplay();
		break;
	case 'z':
		zNear *= 1.1f; zFar *= 1.1f;
		glutPostRedisplay();
		break;
	case 'Z':
		zNear *= 0.9f; zFar *= 0.9f;
		glutPostRedisplay();
		break;

	case ' ':
		// reset values to their defaults
		l = -1.0;
		r = 1.0;
		bottom = -1.0;
		top = 1.0;
		zNear = 1.0;
		zFar = 4.0;
		xoay_roomx = 0;
		xoay_roomy = 0;
		xoay_roomz = 0;
		zzz = 0, yyy = 0, xxx = 0, aaa = 0, bb = 0;
		xoay_demY = 0, xoay_demX = 0, goi1 = 0, goi2 = 0;
		zz = 0, yy = 0, xx = 0, aa = 0, di_chuyen_demX = 0, di_chuyen_demZ = 0, di_chuyen_demY = 0;
		NKTuGiong = 0;
		di_chuyen_banx = 0, di_chuyen_bany = 0, di_chuyen_banz = 0;
		mo_canh_tu_ban = 0;
		keo_ngan_keo_banx = 0, keo_ngan_keo_bany = 0, keo_ngan_keo_banz = 0;
		di_chuyenx = 0, di_chuyeny = 0, di_chuyenz = 0, quay_ghe = 0;
		di_chuyen_tranhz = 0;
		di_chuyen_tu_quan_aox = 0, di_chuyen_tu_quan_aoy = 0, di_chuyen_tu_quan_aoz = 0;
		quay_canh_phai_tu_quan_ao = 0;
		quay_canh_trai_tu_quan_ao = 0;
		di_chuyen_ngan_keo_tu_quan_aoz = 0;
		glutPostRedisplay();
		break;
	}
}




int main(int argc, char** argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Drawing a bedroom");


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
