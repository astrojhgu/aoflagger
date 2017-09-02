#ifndef WRITEFLAGSACTION_H
#define WRITEFLAGSACTION_H

#include "action.h"

#include "../imagesets/imageset.h"

#include <condition_variable>
#include <stack>
#include <mutex>
#include <thread>

#include "../../structures/mask2d.h"

namespace rfiStrategy {

	class WriteFlagsAction : public Action {
		public:
			WriteFlagsAction();
			virtual ~WriteFlagsAction();

			virtual std::string Description()
			{
				return "Write flags to file";
			}

			virtual void Perform(class ArtifactSet &artifacts, ProgressListener &progress);
			virtual ActionType Type() const { return WriteFlagsActionType; }
			virtual void Finish();
			virtual void Sync() { Finish(); Initialize(); }

			void SetMaxBufferItems(size_t maxBufferItems) { _maxBufferItems = maxBufferItems; }
			void SetMinBufferItemsForWriting(size_t minBufferItemsForWriting) { _minBufferItemsForWriting = minBufferItemsForWriting; }
		private:
			struct BufferItem {
				BufferItem(const std::vector<Mask2DCPtr> &masks, const ImageSetIndex &index)
					: _masks(masks), _index(index.Copy())
				{
				}
				BufferItem(const BufferItem &source) : _masks(source._masks), _index(source._index->Copy())
				{
				}
				~BufferItem()
				{
					delete _index;
				}
				void operator=(const BufferItem &source)
				{
					delete _index;
					_masks = source._masks;
					_index = source._index->Copy();
				}
				std::vector<Mask2DCPtr> _masks;
				ImageSetIndex *_index;
			};

			struct FlushFunction
			{
				WriteFlagsAction *_parent;
				void operator()();
			};

			void pushInBuffer(const BufferItem &newItem)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				while(_buffer.size() >= _maxBufferItems)
					_bufferChange.wait(lock);
				_buffer.push(newItem);
				_bufferChange.notify_all();
			}

			std::mutex _mutex;
			std::mutex *_ioMutex;
			std::condition_variable _bufferChange;
			std::thread *_flusher;
			bool _isFinishing;

			size_t _maxBufferItems;
			size_t _minBufferItemsForWriting;

			std::stack<BufferItem> _buffer;
			ImageSet *_imageSet;
	};
}
#endif
