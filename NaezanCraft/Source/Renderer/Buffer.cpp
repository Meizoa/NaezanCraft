#include "../pch.h"
#include "Buffer.h"

Buffer::Buffer()
{
	glGenBuffers(1, &bufferID);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &bufferID);
}

VertexBuffer::VertexBuffer(size_t size, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	//index, size(vec3), type(�� �ϳ��� Ÿ��), normalized(����ȭ ����), stride(�ǳʶ� �Ÿ�), pointer(���� ��ġ)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

VertexBuffer::~VertexBuffer()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

IndexBuffer::IndexBuffer(size_t size, const void* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}