#include "SingleMeshLoader.h"
#include <GLM.h>
// Assimp includes
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

//uncomment if needed
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

SingleMesh::SingleMesh() {}

void SingleMesh::init(const char* mesh_file, const char* tex_file = NULL, const char* normal_file = NULL) {

	if (!load_mesh(mesh_file))
		exit(1);

	if (tex_file != NULL)
	{
		load_texture(tex_file, &tex);
		//glUniform1i(tex, 0);
	}

	if (normal_file != NULL)
	{
		load_texture(normal_file, &norm);
		//glUniform1i(norm, 1);
	}
}

void SingleMesh::initCubeMap(GLfloat vertices[], int num_vertices, string texture)
{
	VAO[0] = NULL;
	GLuint VBO;
	mesh_vertex_count = num_vertices;
	glGenVertexArrays(1, &VAO[0]);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * num_vertices, vertices, GL_STATIC_DRAW);

	for (int i = 0; i < num_vertices * 6; i += 6)
	{
		newpoints.push_back(vertices[i]);
		newpoints.push_back(vertices[i + 1]);
		newpoints.push_back(vertices[i + 2]);
	}

	glBindVertexArray(VAO[0]);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);	//Creating a Texture Buffer

	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X, (TEXTURE_FOLDER + texture + CUBEMAP_RIGHT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, (TEXTURE_FOLDER + texture + CUBEMAP_LEFT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, (TEXTURE_FOLDER + texture + CUBEMAP_TOP).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, (TEXTURE_FOLDER + texture + CUBEMAP_BOT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, (TEXTURE_FOLDER + texture + CUBEMAP_FRONT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, (TEXTURE_FOLDER + texture + CUBEMAP_BACK).c_str());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Changing the many options for textures to suit this situation
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void SingleMesh::loadCubeFace(GLuint textureID, GLenum side, const char* filename)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int w;
	int h;
	int comp;
	unsigned char* image = stbi_load(filename, &w, &h, &comp, STBI_rgb);
	std::string name = filename;
	if (image == NULL)
	{
		fprintf(stderr ,"Failed to load image\n");
		return;
	}
	//else
	//cout << name.substr(name.find_last_of('/') + 1, name.length()) << " [width, height]: [" << w << ", " << h << "]" << endl;
	if ((w & (w - 1)) != 0 || (h & (h - 1)) != 0)
	{
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", filename);
	}

	glTexImage2D(side, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

bool SingleMesh::load_mesh(const char* file_name)
{
	string name = file_name;
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!scene)
	{
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];

		GLfloat* points = NULL; // array of vertex points
		GLfloat* normals = NULL; // array of vertex normals
		GLfloat* texcoords = NULL; // array of texture coordinates
		GLfloat* tangents = NULL;
		//printf("    %i vertices in mesh[%i]\n", mesh->mNumVertices, i);

		/* pass back number of vertex points in mesh */
		mesh_vertex_count = mesh->mNumVertices;

		/* generate a VAO, using the pass-by-reference parameter that we give to the
		function */
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);

		/* we really need to copy out all the data from AssImp's funny little data
		structures into pure contiguous arrays before we copy it into data buffers
		because assimp's texture coordinates are not really contiguous in memory.
		i allocate some dynamic memory to do this. */
		if (mesh->HasPositions())
		{
			points = (GLfloat*)malloc(mesh_vertex_count * 3 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* vp = &(mesh->mVertices[i]);
				points[i * 3] = (GLfloat)vp->x;
				points[i * 3 + 1] = (GLfloat)vp->y;
				points[i * 3 + 2] = (GLfloat)vp->z;
				newpoints.push_back(points[i * 3]);
				newpoints.push_back(points[i * 3 + 1]);
				newpoints.push_back(points[i * 3 + 2]);
			}
			initialpoints = newpoints;
		}
		if (mesh->HasNormals())
		{
			normals = (GLfloat*)malloc(mesh_vertex_count * 3 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* vn = &(mesh->mNormals[i]);
				normals[i * 3] = (GLfloat)vn->x;
				normals[i * 3 + 1] = (GLfloat)vn->y;
				normals[i * 3 + 2] = (GLfloat)vn->z;
				newnormals.push_back(normals[i * 3]);
				newnormals.push_back(normals[i * 3 + 1]);
				newnormals.push_back(normals[i * 3 + 2]);
			}
			initialnormals = newnormals;
		}
		if (mesh->HasTextureCoords(0))
		{
			UVmap = true;
			//cout << "There are texture vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;
			texcoords = (GLfloat*)malloc(mesh_vertex_count * 2 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
				texcoords[i * 2] = (GLfloat)vt->x;
				texcoords[i * 2 + 1] = (GLfloat)vt->y;
				initialtexCoords.push_back(texcoords[i * 2]);
				initialtexCoords.push_back(texcoords[i * 2 + 1]);
				//cout << "[" << texcoords[i * 2] << "," << texcoords[i * 2 + 1] << "]" << endl;
			}
		}
		if (mesh->HasTangentsAndBitangents()) {
			//cout << "There are tangent vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;
			tangents = (GLfloat*)malloc(mesh_vertex_count * 4 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* tangent = &(mesh->mTangents[i]);
				const aiVector3D* bitangent = &(mesh->mBitangents[i]);
				const aiVector3D* normal = &(mesh->mNormals[i]);

				// put the three vectors into my vec3 struct format for doing maths
				glm::vec3 t(tangent->x, tangent->y, tangent->z);
				glm::vec3 n(normal->x, normal->y, normal->z);
				glm::vec3 b(bitangent->x, bitangent->y, bitangent->z);
				// orthogonalise and normalise the tangent so we can use it in something
				// approximating a T,N,B inverse matrix
				glm::vec3 t_i = glm::normalize(t - n * glm::dot(n, t));

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = (glm::dot(glm::cross(n, t), b));
				if (det < 0.0f) {
					det = -1.0f;
				}
				else {
					det = 1.0f;
				}

				// push back 4d vector for inverse tangent with determinant
				tangents[i * 4] = t_i.x;
				tangents[i * 4 + 1] = t_i.y;
				tangents[i * 4 + 2] = t_i.z;
				tangents[i * 4 + 3] = det;
			}
		}
		//else
		//cout << "There are no tangent vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;

		/* copy mesh data into VBOs */
		if (mesh->HasPositions())
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh_vertex_count * 3 * sizeof(GLfloat), points, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);
			free(points);
		}
		if (mesh->HasNormals())
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * mesh_vertex_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);
			free(normals);
		}
		if (mesh->HasTextureCoords(0))
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 2 * mesh_vertex_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(2);
			free(texcoords);
		}
		if (mesh->HasTangentsAndBitangents())
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 4 * mesh_vertex_count * sizeof(GLfloat), tangents, GL_STATIC_DRAW);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(3);
			free(tangents);
		}

	}
	aiReleaseImport(scene);
	return true;
}

bool SingleMesh::load_texture(const char* file_name, GLuint* tex)
{
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data)
	{
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0)
	{
		fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++)
	{
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures(1, tex);
	//glActiveTexture(mode);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}

bool SingleMesh::update_mesh(glm::mat4 &orientation, glm::vec3 &position)
{
	for (int i = 0; i < mesh_vertex_count; i++)
	{
		glm::vec3 vertice = { initialpoints[i * 3], initialpoints[i * 3 + 1], initialpoints[i * 3 + 2] };
		vertice = glm::vec3(orientation * glm::vec4(vertice, 1.0)) + position;
		newpoints[i * 3] = vertice.x;
		newpoints[i * 3 + 1] = vertice.y;
		newpoints[i * 3 + 2] = vertice.z;

		vertice = { initialnormals[i * 3], initialnormals[i * 3 + 1], initialnormals[i * 3 + 2] };
		vertice = glm::vec3(orientation * glm::vec4(vertice, 1.0)) + position;
		newnormals[i * 3] = vertice.x;
		newnormals[i * 3 + 1] = vertice.y;
		newnormals[i * 3 + 2] = vertice.z;
	}



	glBindVertexArray(VAO[0]);
	/* copy mesh data into VBOs */
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_vertex_count * 3 * sizeof(GLfloat), newpoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint vbo2;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 3 * mesh_vertex_count * sizeof(GLfloat), newnormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	return true;
}

bool SingleMesh::scale_mesh(GLfloat scale)
{
	for (int i = 0; i < mesh_vertex_count; i++)
	{
		glm::vec3 vertice = glm::vec3(initialpoints[i * 3], initialpoints[i * 3 + 1], initialpoints[i * 3 + 2]);
		vertice *= scale;
		newpoints[i * 3] = vertice.x;
		newpoints[i * 3 + 1] = vertice.y;
		newpoints[i * 3 + 2] = vertice.z;

		vertice = glm::vec3(initialnormals[i * 3], initialnormals[i * 3 + 1], initialnormals[i * 3 + 2]);
		vertice *= scale;
		vertice = glm::normalize(vertice);
		newnormals[i * 3] = vertice.x;
		newnormals[i * 3 + 1] = vertice.y;
		newnormals[i * 3 + 2] = vertice.z;
	}



	glBindVertexArray(VAO[0]);
	/* copy mesh data into VBOs */
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_vertex_count * 3 * sizeof(GLfloat), newpoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint vbo2;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 3 * mesh_vertex_count * sizeof(GLfloat), newnormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	return true;
}