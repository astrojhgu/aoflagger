#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include "../types.h"

#include "../util/progresslistener.h"

#include <glibmm.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/window.h>

#include <mutex>

class ProgressWindow : public Gtk::Window, public ProgressListener {
	public:
		explicit ProgressWindow(class RFIGuiWindow &parentWindow);
		~ProgressWindow();

		virtual void OnStartTask(const rfiStrategy::Action &action, size_t taskNo, size_t taskCount, const std::string &description, size_t weight=1) final override;
		virtual void OnEndTask(const rfiStrategy::Action &action) final override;
		virtual void OnProgress(const rfiStrategy::Action &action, size_t progress, size_t maxProgress) final override;
		virtual void OnException(const rfiStrategy::Action &action, std::exception &thrownException) final override;
	private:
		void UpdateProgress();
		Glib::Dispatcher _progressChangeSignal;
		std::mutex _mutex;

		Gtk::Label
			_currentTaskTitleLabel, _currentTaskLabel,
			_timeElapsedTitleLabel, _timeElapsedLabel,
			_timeEstimatedTitleLabel, _timeEstimatedLabel;
		
		Gtk::VBox _topBox;
		Gtk::ProgressBar _progressBar;

		double _progressFraction;
		std::vector<std::string> _tasks;
		struct Ratio {
			Ratio(size_t i, size_t c) : index(i), count(c) { }
			size_t index, count;
			};
		std::vector<Ratio> _ratios;
		boost::posix_time::ptime _startTime;
		bool _started;
		bool _exceptionOccured;
		std::string _exceptionType;
		std::string _exceptionDescription;

		class RFIGuiWindow &_parentWindow;
		double _progress;
};

#endif
