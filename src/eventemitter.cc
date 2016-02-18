#include "eventemitter.h"

EventEmitter::EventEmitter(bool notify) : notify_(notify) {
  uv_mutex_init(&list_);
  if(!notify_) {
    uv_mutex_init(&lock_);
    async_ = new uv_async_t();
    async_->data = this;
    uv_async_init(uv_default_loop(), async_,
      reinterpret_cast<uv_async_cb>(EventEmitter::onAsync));
    EventEmitter::SetReference(false);
  }
}

EventEmitter::EventEmitter(EventEmitter* listener) : notify_(true) {
  uv_mutex_init(&list_);
  AddListener(listener);
}

EventEmitter::~EventEmitter() {
  EventEmitter::RemoveAllListeners();
  EventEmitter::Dispose();
  if(!notify_) {
    async_->data = 0;
    uv_close(reinterpret_cast<uv_handle_t*>(async_), EventEmitter::onEnded);
    uv_mutex_destroy(&lock_);
  }
  uv_mutex_destroy(&list_);
}

void EventEmitter::AddListener(EventEmitter *listener) {
  bool found = false;
  std::vector<EventEmitter*>::iterator index;
  if(listener && listener != this) {
    uv_mutex_lock(&list_);
    for(index = listeners_.begin(); index < listeners_.end(); index++) {
      if((*index) == listener) {
        found = true;
        break;
      }
    }
    if(!found) {
      listeners_.push_back(listener);
      listener->AddParent(this);
    }
    uv_mutex_unlock(&list_);
  }
}

void EventEmitter::RemoveListener(EventEmitter *listener) {
  std::vector<EventEmitter*>::iterator index;
  if(listener && listener != this) {
    uv_mutex_lock(&list_);
    for(index = listeners_.begin(); index < listeners_.end(); index++) {
      if((*index) == listener) {
        listeners_.erase(index);
        listener->RemoveParent(this);
        break;
      }
    }
    uv_mutex_unlock(&list_);
  }
}

void EventEmitter::RemoveAllListeners() {
  std::vector<EventEmitter*>::iterator index;
  uv_mutex_lock(&list_);
  for(index = listeners_.begin(); index < listeners_.end(); index++) {
    (*index)->RemoveParent(this);
    listeners_.erase(index);
  }
  uv_mutex_unlock(&list_);
}

void EventEmitter::Dispose() {
  if(!notify_) {
    while(!events_.empty()) {
      rtc::scoped_refptr<Event> event = events_.front();
      events_.pop();
    }
  }
}

void EventEmitter::SetReference(bool alive) {
  if(!notify_) {
    uv_mutex_lock(&lock_);
    if(alive) {
      uv_ref(reinterpret_cast<uv_handle_t*>(async_));
    } else {
      uv_unref(reinterpret_cast<uv_handle_t*>(async_));
    }
    uv_mutex_unlock(&lock_);
  }
}

void EventEmitter::Emit(int event) {
  EventEmitter::Emit(new rtc::RefCountedObject<Event>(event));
}

void EventEmitter::Emit(rtc::scoped_refptr<Event> event) {
  if(event.get()) {
    if(!notify_) {
      uv_mutex_lock(&lock_);
      events_.push(event);
      uv_async_send(async_);
      uv_mutex_unlock(&lock_);
    }
    uv_mutex_lock(&list_);
    std::vector<EventEmitter*>::iterator index;
    for(index = listeners_.begin(); index < listeners_.end(); index++) {
      (*index)->Emit(event);
    }
    uv_mutex_unlock(&list_);
  }
}

void EventEmitter::AddParent(EventEmitter *listener) {
  uv_mutex_lock(&list_);
  parents_.push_back(listener);
  uv_mutex_unlock(&list_);
}

void EventEmitter::RemoveParent(EventEmitter *listener) {
  std::vector<EventEmitter*>::iterator index;
  uv_mutex_lock(&list_);
  for(index = listeners_.begin(); index < listeners_.end(); index++) {
    if((*index) == listener) {
      listeners_.erase(index);
    }
  }
  uv_mutex_unlock(&list_);
}

void EventEmitter::onAsync(uv_async_t *handle, int status) {
  EventEmitter *self = static_cast<EventEmitter*>(handle->data);
  if(self) {
    self->DispatchEvents();
  }
}

void EventEmitter::onEnded(uv_handle_t *handle) {
  uv_async_t* async = reinterpret_cast<uv_async_t*>(handle);
  if(async) {
    delete async;
  }
}

void EventEmitter::DispatchEvents() {
  uv_mutex_lock(&lock_);
  while(!events_.empty()) {
    rtc::scoped_refptr<Event> event = events_.front();
    events_.pop();
    uv_mutex_unlock(&lock_);
    if(event.get()) {
      On(event);
    }
    uv_mutex_lock(&lock_);
  }
  uv_mutex_unlock(&lock_);
}
