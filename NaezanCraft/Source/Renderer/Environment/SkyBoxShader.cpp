#include "../../pch.h"
#include "SkyBoxShader.h"
#include "../../World/Camera.h"
#include "../../World/Scene.h"
#include "../../Util/Math.h"

#include "../../Input/InputSystem.h"

SkyBoxShader::SkyBoxShader(const std::string& path, ShaderType shadertype) :
	Shader(path, shadertype)
{
}

void SkyBoxShader::GetUniform(uint32_t shaderID)
{
	dayTime = glGetUniformLocation(shaderID, "dayTime");
	light = glGetUniformLocation(shaderID, "light");
	projectionViewMatrix = glGetUniformLocation(shaderID, "projectionview");
	modelMatrix = glGetUniformLocation(shaderID, "model");
}

void SkyBoxShader::Update(std::weak_ptr<Camera>& camera, const glm::mat4& _modelMatrix)
{
	worldTime += .1f;
	if (worldTime > 23999) {
		worldTime = 0;
	}

	if (Input::GetIsKeyPressed(GLFW_KEY_HOME))
	{
		worldTime+= 100.f;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_END))
	{
		worldTime -= 100.f;
	}

	//0 -> ����6��, 6000 ����, 12000 ����6��, 18000 ��12��
	if (worldTime < 3000) { //����6��~����9�ö�� ���������
		sunIntensity = blend(0.6f, 1.0f, (float)worldTime / 3000);
	}
	if (worldTime > 3000 && worldTime < 12000) { //����9��~����6�� �ִ� ���
		sunIntensity = 1.0f;
	}
	if (worldTime > 12000 && worldTime < 15000) { //����6��~����9�� ���� ��ο���
		sunIntensity = blend(1.0f, 0.6f, (float)((float)worldTime - 12000) / 3000);
	}
	if (worldTime > 15000 && worldTime < 18000) { //����9��~��12�� �� ��ο���
		sunIntensity = blend(0.6f, 0.1f, (float)((float)worldTime - 15000) / 3000);
	}
	if (worldTime > 18000 && worldTime < 21000) { //��12��~����3�� ��� ����ġ
		sunIntensity = 0.1f;
	}
	if (worldTime > 21000 && worldTime < 24000) { //����3��~����6�� �����
		sunIntensity = blend(0.1f, 0.6f, (float)((float)worldTime - 21000) / 3000);
	}

	glUniformMatrix4fv(projectionViewMatrix, 1, GL_FALSE, glm::value_ptr(camera.lock()->GetViewProjectionMatrix()));
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
	//������������ ���� ��ġ ����
	glUniform1f(dayTime, worldTime);
	glUniform1f(light, 1);
}
