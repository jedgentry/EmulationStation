#pragma once

#include "views/gamelist/IGameListView.h"
#include "views/gamelist/Keyboard.h"
#include "components/TextComponent.h"
#include "components/ImageComponent.h"

class ISimpleGameListView : public IGameListView
{
public:
	ISimpleGameListView(Window* window, FileData* root);
	virtual ~ISimpleGameListView() { }

	// Called when a new file is added, a file is removed, a file's metadata changes, or a file's children are sorted.
	// NOTE: FILE_SORTED is only reported for the topmost FileData, where the sort started.
	//       Since sorts are recursive, that FileData's children probably changed too.
	virtual void onFileChanged(FileData* file, FileChangeType change);
	
	// Called whenever the theme changes.
	virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme);

	virtual FileData* getCursor() = 0;
	virtual void setCursor(FileData*) = 0;

	virtual bool input(InputConfig* config, Input input) override;
	//Our draw method for the keyboard.
	virtual void render(const Eigen::Affine3f& parentTrans);
	virtual void update(int deltaTime);
	

protected:
	virtual void populateList(const std::vector<FileData*>& files) = 0;
	virtual void launch(FileData* game) = 0;

	TextComponent mHeaderText;
	TextComponent mKeyboardText;

	ImageComponent mHeaderImage;
	ImageComponent mBackground;
	
	ThemeExtras mThemeExtras;

	std::stack<FileData*> mCursorStack;

private:
	Keyboard* mKeyboard;
};

