#pragma once
#include "../icode_generator.h"

namespace Halley
{
	class CodegenCPP : public ICodeGenerator
	{
	public:
		String getDirectory() const override { return "cpp"; }
		CodegenLanguage getLanguage() const override { return CodegenLanguage::CPlusPlus; }

		CodeGenResult generateComponent(ComponentSchema component) override;
		CodeGenResult generateSystem(SystemSchema system) override;
		CodeGenResult generateMessage(MessageSchema message) override;

		CodeGenResult generateRegistry(const std::vector<ComponentSchema>& components, const std::vector<SystemSchema>& systems) override;

	private:
		std::vector<String> generateComponentHeader(ComponentSchema component);
		std::vector<String> generateSystemHeader(SystemSchema system);
		std::vector<String> generateMessageHeader(MessageSchema message);
	
	};
}