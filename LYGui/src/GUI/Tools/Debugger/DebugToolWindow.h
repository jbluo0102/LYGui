#pragma once
#include <GUI/Tools/ImGuiToolWindow.h>

class UDebugToolWindow :
	public UImGuiToolWindow
{
	std::future<void> m_LoadFuture;
	std::future<void> m_AllocateFuture;

	ImGuiTreeNodeFlags m_TreeNodeFlags;

	void RandomTask();

public:
	UDebugToolWindow();
protected:
	/** Override **/
	virtual void RenderTool() override;
	virtual void RenderDetail() override;
	virtual void RenderViewport() override;
private:
	/** Testing **/
	void RenderHardware();
	void RenderLayout();
	void RenderUIDesign();
	void RenderFuncMap();
	void RenderFunctional();
};
