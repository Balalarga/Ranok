#include "MImageComputeModule.h"

#include <iostream>

#include "Config/IConfig.h"

#include "Graphics/Rendering/OpenglRender.h"
#include "Graphics/Rendering/VoxelModel.h"

#include "Localization/LocalizationSystem.h"

#include "Utils/FileUtils.h"
#include "Utils/WindowsUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{
DEFINE_LOCTEXT(MimageComputeModuleName, "MimageCompute")
DEFINE_LOCTEXT(MImageOpenFile, "Open")


class MImageModuleConfig: public IConfig
{
public:
	MImageModuleConfig(): IConfig("Modules/MimageConfig")
	{
	}

	void Serialize(JsonArchive& archive) override
	{
		archive.Serialize("viewSize", viewSize);
	}

	bool operator!=(const IConfig& oth) override
	{
		const auto casted = dynamic_cast<const MImageModuleConfig*>(&oth);
		assert(casted);
		return viewSize == casted->viewSize;
	}

	glm::vec2 viewSize = {1, 1};
};

std::shared_ptr<MImageModuleConfig> GetConfig()
{
	static auto config = std::make_shared<MImageModuleConfig>();
	return config;
}

MImageComputeModule::MImageComputeModule():
	IEditorModule(LOCTEXTSTR(MimageComputeModuleName)),
    _viewport({800, 600}),
	_imageGradient({
		ColorFromUint(255, 255, 0, 20),
		ColorFromUint(0, 255, 162, 20),
		ColorFromUint(0, 0, 255, 20),
		ColorFromUint(255, 145, 0, 20),
		ColorFromUint(214, 0, 255, 20)
	}),
	_camera(45.f, 800.f / 600.f, 0.01f, 10000.f)
{
	SetNoClosing(true);
	bWorks = true;
	_viewport.Create();
}

void MImageComputeModule::RenderWindowContent()
{
	ImGui::BeginGroup();
	if (ImGui::Button(LOCTEXT(MImageOpenFile)))
	{
		const std::string filepathStr = OpenFileDialog("*.bin\0");
		if (!filepathStr.empty())
		{
            _imageData.Clear();
            std::ifstream file(filepathStr, std::ios_base::binary);
			file >> _space;
			size_t totalDataSize = _space.GetTotalPartition();
            const std::vector<size_t>& dataSize = _space.GetPartition();
			_imageData.Resize(dataSize);
			_imageData.ReadSome(file, totalDataSize);
			file.close();

			Logger::Log(fmt::format("Mimage read ({} dims)", fmt::join(_imageData.GetDimensions(), ", ")));
            _model.reset(VoxelModel::Make(_space, _imageData, _imageGradient));
			_model->Material().SetCameraMatrix(_camera.ViewProjectionMatrix());
		}
	}
	ImGui::EndGroup();

	static float w = ImGui::GetWindowContentRegionMax().x / 3.f;
	float trueH = ImGui::GetWindowContentRegionMax().y;
	float trueW = w >= 0 ? w : 1;

	ImGui::BeginChild("WorkingChild");
	ImGui::BeginChild("TextEditorChild", ImVec2(w, 0), true);

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::InvisibleButton("vsplitter", ImVec2(5.0f, ImGui::GetWindowContentRegionMax().y));
	const bool bIsActive = ImGui::IsItemActive();
	const bool bIsHovered = ImGui::IsItemHovered();
	if (bIsActive)
		w += ImGui::GetIO().MouseDelta.x;
	if (bIsActive || bIsHovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::BeginChild("ViewportChild", ImGui::GetContentRegionAvail(), true);
	_viewport.Resize({ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y});
	if (_model)
	{
		_viewport.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_model->Render();
		_viewport.Release();
	}
	ImGui::Image((void*)(intptr_t)_viewport.GetTextureId(),
				 ImGui::GetWindowSize(),
				 ImVec2(0, 1),
				 ImVec2(1, 0));

	if (ImGui::IsItemHovered() && _model)
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			if (mouseDelta.x != 0 || mouseDelta.y != 0)
			{
				_camera.Rotate({mouseDelta.y / 100.f, mouseDelta.x / 100.f, 0});
				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
			}
		}
		if (ImGui::GetIO().MouseWheel != 0)
		{
			_camera.MoveForward(ImGui::GetIO().MouseWheel / 10.f);
		}

		if (ImGui::IsKeyDown(ImGuiKey_W))
		{
			_camera.MoveForward(-0.1f);
		}
		else if (ImGui::IsKeyDown(ImGuiKey_S))
		{
			_camera.MoveForward(0.1f);
		}
		if (ImGui::IsKeyDown(ImGuiKey_A))
		{
			_camera.MoveRight(-0.1f);
		}
		else if (ImGui::IsKeyDown(ImGuiKey_D))
		{
			_camera.MoveRight(0.1f);
		}
		_model->Material().SetCameraMatrix(_camera.ViewProjectionMatrix());
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(1);

	ImGui::EndChild();
}
}
