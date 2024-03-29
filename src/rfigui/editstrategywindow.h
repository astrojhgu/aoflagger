#ifndef EDITSTRATEGYWINDOW_H
#define EDITSTRATEGYWINDOW_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/window.h>

#include "../strategy/control/types.h"

#include <memory>

class EditStrategyWindow : public Gtk::Window
{
	public:
		explicit EditStrategyWindow(class RFIGuiController& guiController, class StrategyController &strategyController);

		void AddAction(std::unique_ptr<rfiStrategy::Action> newAction);
		void UpdateAction(rfiStrategy::Action *action);
	private:
		rfiStrategy::Action *GetSelectedAction();
		size_t GetSelectedActionChildIndex();
		void initEditButtons();
		void initLoadDefaultsButtons();
		void fillStore();
		void fillStore(Gtk::TreeModel::Row &row, rfiStrategy::Action &action, size_t childIndex);

		void onRemoveActionClicked();
		void onMoveUpClicked();
		void onMoveDownClicked();
		void onSelectionChanged();

		void onSaveClicked();
		void onOpenClicked();

		void onLoadEmptyClicked();
		void onWizardClicked();
		
		void onStrategyChanged();

		void clearRightFrame();
		void selectAction(rfiStrategy::Action *action);
		void showRight(Gtk::Frame *newFrame)
		{
			clearRightFrame();
			_rightFrame = newFrame; 
			_paned.add2(*_rightFrame);
			_rightFrame->show();
		}
		Gtk::TreeModel::Row findActionRow(rfiStrategy::Action *action);
		//void addContainerBetween(rfiStrategy::ActionContainer &root, std::unique_ptr<rfiStrategy::ActionContainer> newContainer);

		class ModelColumns : public Gtk::TreeModelColumnRecord
		{
		public:
		
			ModelColumns()
				{ add(action); add(description); add(childIndex); }
		
			Gtk::TreeModelColumn<rfiStrategy::Action *> action;
			Gtk::TreeModelColumn<Glib::ustring> description;
			Gtk::TreeModelColumn<size_t> childIndex;
		};
		
		class StrategyController &_strategyController;

		Gtk::Paned _paned;
		Gtk::VBox _strategyBox;
		rfiStrategy::Strategy *_strategy;
		Gtk::ButtonBox _strategyEditButtonBox, _strategyFileButtonBox, _strategyLoadDefaultsButtonBox;
		Gtk::MenuToolButton _addActionButton;
		Gtk::Button _removeActionButton, _moveUpButton, _moveDownButton;
		Gtk::Button _loadEmptyButton, _wizardButton, _saveButton, _openButton;
		ModelColumns _columns;
		Gtk::ScrolledWindow _viewScrollWindow;
		Gtk::TreeView _view;
		Glib::RefPtr<Gtk::TreeStore> _store;
		std::unique_ptr<Gtk::Menu> _addMenu;
		bool _disableUpdates;

		class RFIGuiController& _guiController;
		Gtk::Frame *_rightFrame;
		std::unique_ptr<Gtk::Window> _wizardWindow;
};

#endif
