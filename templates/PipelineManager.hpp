#ifndef __PIPELINE_MANAGER_HPP__
#define __PIPELINE_MANAGER_HPP__
#include <memory>

struct SRect {
	long left;
	long top;
	long right;
	long bottom;
};

class PipelineManager {
public:
	virtual ~PipelineManager() = default;

	inline static void Init(std::unique_ptr<PipelineManager> instance) {
		s_instance = std::move(instance);
	}

	inline static PipelineManager* GetRef() noexcept {
		return s_instance.get();
	}

	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual SRect GetRenderAreaRECT() = 0;

private:
	static std::unique_ptr<PipelineManager> s_instance;
};
#endif
