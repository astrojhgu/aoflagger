#include "progresswindow.h"

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <gtkmm/messagedialog.h>

#include "rfiguiwindow.h"

ProgressWindow::ProgressWindow(class RFIGuiWindow &parentWindow) :
	_currentTaskTitleLabel("Current task:"),
	_currentTaskLabel("-"),
	_timeElapsedTitleLabel("Time elapsed:"),
	_timeElapsedLabel("-"),
	_timeEstimatedTitleLabel("Estimated time:"),
	_timeEstimatedLabel("-"),
	_started(false),
	_exceptionOccured(false),
	_parentWindow(parentWindow),
	_progress(0.0)
{
	set_default_size(400, 300);

	_topBox.pack_start(_currentTaskTitleLabel);
	_currentTaskTitleLabel.show();

	_topBox.pack_start(_currentTaskLabel);
	_currentTaskLabel.show();

	_topBox.pack_start(_timeElapsedTitleLabel);
	_timeElapsedTitleLabel.show();

	_topBox.pack_start(_timeElapsedLabel);
	_timeElapsedLabel.show();

	_topBox.pack_start(_timeEstimatedTitleLabel);
	_timeEstimatedTitleLabel.show();

	_topBox.pack_start(_timeEstimatedLabel);
	_timeEstimatedLabel.show();

	_topBox.pack_start(_progressBar);
	_progressBar.show();

	add(_topBox);
	_topBox.show();

	_progressChangeSignal.connect(sigc::mem_fun(*this, &ProgressWindow::UpdateProgress));
}


ProgressWindow::~ProgressWindow()
{
}

void ProgressWindow::UpdateProgress()
{
	if(!_started)
	{
		_startTime = boost::posix_time::microsec_clock::local_time();
		_started = true;
	}

	std::unique_lock<std::mutex> lock(_mutex);

	if(_exceptionOccured)
	{
		_exceptionOccured = false;
		Gtk::MessageDialog dialog(*this, std::string("An exception was thrown of type '") + _exceptionType + ("': ") + _exceptionDescription, false, Gtk::MESSAGE_ERROR);
		lock.unlock();
		dialog.run();
		lock.lock();
	}

	std::stringstream timeStr;
	timeStr << (boost::posix_time::microsec_clock::local_time() - _startTime);
	_timeElapsedLabel.set_text(timeStr.str());
	
	if(_tasks.size() == 0)
	{
		// The task has completed

		lock.unlock();

		_currentTaskLabel.set_text("-");
		_timeEstimatedLabel.set_text("-");
		_progressBar.set_fraction(1.0);
		
		// Parent might delete this window in this call -- don't do anything after.
		_parentWindow.onExecuteStrategyFinished();
	} else
	{
		std::vector<std::string>::const_iterator taskDesc =_tasks.begin();
		std::stringstream str;
		str << *taskDesc;
		++taskDesc;
		for(;taskDesc!=_tasks.end();++taskDesc)
			str << "->\n  " << *taskDesc;
	
		double progress = _progress;
		lock.unlock();

		_currentTaskLabel.set_text(str.str());
		_progressBar.set_fraction(progress);

		if(progress > 0.0)
		{
			std::stringstream estimatedTimeStr;
			boost::posix_time::time_duration estimated =
				(boost::posix_time::microsec_clock::local_time() - _startTime) * (int) (1000000.0*(1.0-progress))/(int) (1000000.0*progress);
			estimated = boost::posix_time::seconds(estimated.total_seconds());
			estimatedTimeStr << estimated;
			_timeEstimatedLabel.set_text(estimatedTimeStr.str());
		}
	}
}

void ProgressWindow::OnStartTask(const rfiStrategy::Action &action, size_t taskNo, size_t taskCount, const std::string &description, size_t weight)
{
	std::unique_lock<std::mutex> lock(_mutex);
	ProgressListener::OnStartTask(action, taskNo, taskCount, description, weight);
	std::stringstream str;
	str << "[" << taskNo << "/" << taskCount << "] " << description;
	_tasks.push_back(str.str());
	_ratios.push_back(Ratio(taskNo, taskCount));
	_progress = TotalProgress();
	lock.unlock();

	_progressChangeSignal();
}

void ProgressWindow::OnEndTask(const rfiStrategy::Action &action)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_tasks.pop_back();
	_ratios.pop_back();
	_progress = TotalProgress();
	ProgressListener::OnEndTask(action);
	lock.unlock();

	_progressChangeSignal();
}

void ProgressWindow::OnProgress(const rfiStrategy::Action &action, size_t progress, size_t maxProgress)
{
	std::unique_lock<std::mutex> lock(_mutex);
	ProgressListener::OnProgress(action, progress, maxProgress);
	_progress = TotalProgress();
	lock.unlock();

	_progressChangeSignal();
}

void ProgressWindow::OnException(const rfiStrategy::Action &, std::exception &thrownException)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_exceptionOccured = true;
	_exceptionDescription = thrownException.what();
	_exceptionType = typeid(thrownException).name();
	lock.unlock();
	_progressChangeSignal();
}

