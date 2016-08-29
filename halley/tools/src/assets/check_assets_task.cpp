#include <set>
#include "halley/tools/assets/check_assets_task.h"
#include "halley/tools/assets/import_assets_task.h"
#include "halley/tools/project/project.h"
#include "halley/tools/assets/import_assets_database.h"
#include "halley/tools/assets/delete_assets_task.h"
#include <boost/filesystem/operations.hpp>

using namespace Halley;
using namespace std::chrono_literals;

CheckAssetsTask::CheckAssetsTask(Project& project)
	: EditorTask("Check assets", true, false)
	, project(project)
{}

void CheckAssetsTask::run()
{
	while (!isCancelled()) {
		checkAllAssets();
		do {
			std::this_thread::sleep_for(50ms);
		} while (hasPendingTasks());
	}
}

void CheckAssetsTask::checkAllAssets()
{
	auto& db = project.getImportAssetsDatabase();
	db.markAllAsMissing();

	Vector<AssetToImport> assets;
	std::set<Path> included;

	// Enumerate all potential assets
	for (auto srcPath : { project.getAssetsSrcPath(), project.getSharedAssetsSrcPath() }) {
		for (auto filePath : FileSystem::enumerateDirectory(srcPath)) {
			if (included.find(filePath) == included.end()) {
				included.insert(filePath);
				assets.emplace_back(filePath, srcPath, FileSystem::getLastWriteTime(srcPath / filePath));
				db.markAsPresent(filePath);
			}
		}
	}

	// Missing
	deleteMissing(db.getAllMissing());

	// Added
	checkAssets(assets);
}

void CheckAssetsTask::checkAssets(const std::vector<AssetToImport>& assets)
{
	auto& db = project.getImportAssetsDatabase();
	Vector<AssetToImport> toImport;

	for (auto &a : assets) {
		if (db.needsImporting(a.name, a.fileTime)) {
			toImport.push_back(a);
		}
	}

	if (!toImport.empty()) {
		addPendingTask(EditorTaskAnchor(std::make_unique<ImportAssetsTask>(project, std::move(toImport))));
	}	
}

void CheckAssetsTask::deleteMissing(const std::vector<Path>& paths)
{
	if (!paths.empty()) {
		addPendingTask(EditorTaskAnchor(std::make_unique<DeleteAssetsTask>(project, paths)));
	}
}