#ifndef MSOPTIONWINDOW_H
#define MSOPTIONWINDOW_H

#include <string>

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>

class MSOptionWindow : public Gtk::Window {
	public:
		MSOptionWindow(class RFIGuiController &controller, const std::vector<std::string>& filenames);
		~MSOptionWindow();
		void onOpen();
	private:
		void initDataTypeButtons();
		void initPolarisationButtons();
		void initReadingModeButtons();

		class RFIGuiController &_controller;
		const std::vector<std::string> _filenames;

		Gtk::ButtonBox _bottomButtonBox;
		Gtk::VBox _leftVBox, _rightVBox;
		Gtk::HBox _topHBox;
		Gtk::Button _openButton;
		Gtk::Frame _dataKindFrame, _polarisationFrame, _readingModeFrame;
		Gtk::VBox _dataKindBox, _polarisationBox, _readingModeBox;
		Gtk::HBox _otherColumnBox, _intervalBox;
		Gtk::RadioButton _observedDataButton, _correctedDataButton, _modelDataButton, _residualDataButton, _otherColumnButton;
		Gtk::Entry _otherColumnEntry, _intervalStartEntry, _intervalEndEntry;
		Gtk::RadioButton _allDipolePolarisationButton, _autoDipolePolarisationButton, _stokesIPolarisationButton;
		Gtk::RadioButton _directReadButton, _indirectReadButton, _memoryReadButton;
		Gtk::CheckButton _intervalButton, _combineSPWsButton, _readUVWButton, _loadOptimizedStrategy;
};

#endif
