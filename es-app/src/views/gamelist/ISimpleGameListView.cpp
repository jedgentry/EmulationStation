#include "views/gamelist/ISimpleGameListView.h"
#include "ThemeData.h"
#include "Window.h"
#include "views/ViewController.h"
#include "Sound.h"
#include "Settings.h"

ISimpleGameListView::ISimpleGameListView(Window* window, FileData* root) : IGameListView(window, root),
mHeaderText(window), mHeaderImage(window), mBackground(window), mThemeExtras(window), mKeyboardText(window)
{
	mKeyboard = make_unique<Keyboard>("en", SystemData::sSystemVector);
	mHeaderText.setText("Logo Text");
	mHeaderText.setSize(mSize.x(), 0);
	mHeaderText.setPosition(0, 0);
	mHeaderText.setAlignment(ALIGN_CENTER);

	mKeyboardText.setText(mKeyboard->getKeys());
	mKeyboardText.setSize(mSize.x(), 0);
	mKeyboardText.setPosition(0, mSize.y() * 0.15f);
	mKeyboardText.setColor(0x00000088);
	mKeyboardText.setAlignment(ALIGN_CENTER);

	mHeaderImage.setResize(0, mSize.y() * 0.185f);
	mHeaderImage.setOrigin(0.5f, 0.0f);
	mHeaderImage.setPosition(mSize.x() / 2, 0);

	mBackground.setResize(mSize.x(), mSize.y());

	addChild(&mKeyboardText);
	addChild(&mHeaderText);
	addChild(&mBackground);
	addChild(&mThemeExtras);
}

void ISimpleGameListView::onThemeChanged(const std::shared_ptr<ThemeData>& theme)
{
	using namespace ThemeFlags;
	mBackground.applyTheme(theme, getName(), "background", ALL);
	mHeaderImage.applyTheme(theme, getName(), "logo", ALL);
	mHeaderText.applyTheme(theme, getName(), "logoText", ALL);
	mThemeExtras.setExtras(ThemeData::makeExtras(theme, getName(), mWindow));

	if(mHeaderImage.hasImage())
	{
		removeChild(&mHeaderText);
		addChild(&mHeaderImage);
	}else{
		addChild(&mHeaderText);
		removeChild(&mHeaderImage);
	}
}

void ISimpleGameListView::onFileChanged(FileData* file, FileChangeType change)
{
	// we could be tricky here to be efficient;
	// but this shouldn't happen very often so we'll just always repopulate
	FileData* cursor = getCursor();
	populateList(cursor->getParent()->getChildren());
	setCursor(cursor);
}
//Handles the input for the game views.
bool ISimpleGameListView::input(InputConfig* config, Input input)
{
	if (input.value != 0) //Redirect input if keyboard is open.
	{
		if (config->isMappedTo("a", input) && !mKeyboard->isOpen())
		{
			FileData* cursor = getCursor();
			if(cursor->getType() == GAME)
			{
				Sound::getFromTheme(getTheme(), getName(), "launch")->play();
				launch(cursor);
			}else{
				// it's a folder
				if(cursor->getChildren().size() > 0)
				{
					mCursorStack.push(cursor);
					populateList(cursor->getChildren());
				}
			}
				
			return true;
		}else if(config->isMappedTo("b", input) && !mKeyboard->isOpen())
		{
			if(mCursorStack.size())
			{
				populateList(mCursorStack.top()->getParent()->getChildren());
				setCursor(mCursorStack.top());
				mCursorStack.pop();
				Sound::getFromTheme(getTheme(), getName(), "back")->play();
			}else{
				onFocusLost();
				ViewController::get()->goToSystemView(getCursor()->getSystem());
			}

			return true;
		}
		else if (config->isMappedTo("y", input)) //User has opened up the keyboard.
		{
			mKeyboard->setKeyboardIsOpen(true);
			return true; //Return like other inputs.
		}
		else if(config->isMappedTo("right", input))
		{
			if(Settings::getInstance()->getBool("QuickSystemSelect"))
			{
				onFocusLost();
				ViewController::get()->goToNextGameList();
				return true;
			}
		}else if(config->isMappedTo("left", input))
		{
			if(Settings::getInstance()->getBool("QuickSystemSelect"))
			{
				onFocusLost();
				ViewController::get()->goToPrevGameList();
				return true;
			}
		}
	}
	//Redirect input to keyboard.
	//There needs to be a sleep timer on the y option.
	//THERE IS ALREADY A JUMP TO LETTER FUNCTION IN GUIGAMELISTOPTIONS.cpp
	if (mKeyboard->isOpen())
	{
		//Close keyboard.
		if (config->isMappedTo("b", input) && input.value || config->isMappedTo("y", input) && input.value)
		{
			mKeyboard->setKeyboardIsOpen(false);
		}
		else if (config->isMappedTo("left", input) && input.value)
		{
			mKeyboard->iterateLeft();
			mKeyboardText.setText(mKeyboard->getKeys());
		}
		else if (config->isMappedTo("right", input) && input.value)
		{
			mKeyboard->iterateRight();
			mKeyboardText.setText(mKeyboard->getKeys());
		}
		else if (config->isMappedTo("a", input) && input.value)
		{
			if (mKeyboard->goToKey(mRoot) && mKeyboard->getPrevChoice() !=
				getCursor()->getCleanName()[0])
			{ //Not nullptr
				setCursor(mKeyboard->goToKey(mRoot));
			}
			else
			{
				FileData* cursor = getCursor();
				if (cursor->getType() == GAME)
				{
					Sound::getFromTheme(getTheme(), getName(), "launch")->play();
					launch(cursor);
				}
				else{
					// it's a folder
					if (cursor->getChildren().size() > 0)
					{
						mCursorStack.push(cursor);
						populateList(cursor->getChildren());
					}
				}
			}
		}  
	}

	return IGameListView::input(config, input);
}

void ISimpleGameListView::render(const Eigen::Affine3f& parentTrans)
{
	IGameListView::render(parentTrans);
	if (mKeyboard->isOpen())
		mKeyboardText.render(parentTrans * getTransform());
}

void ISimpleGameListView::update(int deltaTime)
{
}
