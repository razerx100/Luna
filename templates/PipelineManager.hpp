#ifndef __PIPELINE_MANAGER_HPP__
#define __PIPELINE_MANAGER_HPP__
#include <memory>
#include <GenericSingleton.hpp>

struct SRect {
	long left;
	long top;
	long right;
	long bottom;
};

class PipelineManager : public GenericSingleton<PipelineManager> {
public:
	virtual ~PipelineManager() = default;

	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual SRect GetMonitorCoordinates() = 0;
};
#endif
