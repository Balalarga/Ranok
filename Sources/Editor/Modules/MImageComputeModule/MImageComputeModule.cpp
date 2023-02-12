#include "MImageComputeModule.h"

#include "Graphics/Rendering/VoxelModel.h"

#include "Localization/LocalizationSystem.h"

#include "Utils/FileUtils.h"
#include "Utils/WindowsUtils.h"

namespace Ranok
{
DEFINE_LOCTEXT(MimageComputeModuleName, "MimageCompute")
DEFINE_LOCTEXT(MImageOpenFile, "Open")

MImageComputeModule::MImageComputeModule():
	IEditorModule(LOCTEXTSTR(MimageComputeModuleName)),
    _viewport({800, 600}),
	_imageGradient({
				   ColorFromUint(255, 255, 0,   20),
				   ColorFromUint(0,   255, 162, 20),
				   ColorFromUint(0,   0,   255, 20),
				   ColorFromUint(255, 145, 0,   20),
				   ColorFromUint(214, 0,   255, 20)})
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

	ImGui::Image((void*)(intptr_t)_viewport.GetTextureId(),
				 ImGui::GetWindowSize(),
				 ImVec2(0, 1),
				 ImVec2(1, 0));
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			if (mouseDelta.x != 0 || mouseDelta.y != 0)
			{
				if (_model)
				{
					_viewport.Bind();
					_model->Render();
					_viewport.Release();
				}

			}
			// _viewport.MouseMoved(mouseDelta);
			// if (!Math::IsZero(mouseDelta.x) || !Math::IsZero(mouseDelta.y))
				// ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
		}
		// if (!Math::IsZero(ImGui::GetIO().MouseWheel))
			// _viewport.Zoom(ImGui::GetIO().MouseWheel * (ImGui::IsKeyDown(ImGuiKey_LeftShift) ? 5.f : .5f));
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(1);

	ImGui::EndChild();
}
}
