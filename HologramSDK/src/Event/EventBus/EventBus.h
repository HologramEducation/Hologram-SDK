/*
 * Copyright (c) 2014, Dan Quist
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _SRC_EVENT_EVENT_BUS_H_
#define _SRC_EVENT_EVENT_BUS_H_

#include "EventHandler.h"
#include "Event.h"
#include "EventRegistration.h"

#include <list>
#include <typeinfo>
#include <unordered_map>


 /**
  * \brief An Event system that allows decoupling of code through synchronous events
  *
  */
class EventBus {
public:
	/**
	* \brief Empty virtual destructor
	*/
	virtual ~EventBus() { }

	/**
	 * \brief Returns the EventBus singleton instance
	 *
	 * Creates a new instance of the EventBus if hasn't already been created
	 *
	 * @return The singleton instance
	 */
	static std::shared_ptr<EventBus> const GetInstance() {
		if (ptrInstance == nullptr) {
			ptrInstance = std::shared_ptr<EventBus>(new EventBus());
		}

		return ptrInstance;
	}


	/**
	 * \brief Registers a new event handler to the EventBus
	 *
	 * @param handler The event handler class
	 * @return An EventRegistration pointer which can be used to unregister the event handler
	 */
	template <class T>
	static HandlerRegistrationPtr const AddHandler(EventHandler<T> & handler) {
		std::shared_ptr<EventBus> pInstance = GetInstance();

		// Fetch the list of event pairs unique to this event type
		std::shared_ptr<std::list<EventRegistrationPtr>> pRegistrations = pInstance->handlers[typeid(T)];

		// Create a new collection instance for this type if it hasn't been created yet
		if (pRegistrations == nullptr) {
			pRegistrations = std::shared_ptr<std::list<EventRegistrationPtr>>(new std::list<EventRegistrationPtr>());
			pInstance->handlers[typeid(T)] = pRegistrations;
		}

		// Create a new EventPair instance for this registration.
		// This will group the handler, sender, and registration object into the same class
		EventRegistrationPtr ptr_registration = EventRegistrationPtr(new EventRegistration(static_cast<void*>(&handler), pRegistrations));

		// Add the registration object to the collection
		pRegistrations->push_back(ptr_registration);

		return ptr_registration;
	}


	/**
	 * \brief Fires an event
	 *
	 * @param e The event to fire
	 */
	static void FireEvent(const Event & e) {
		std::shared_ptr<EventBus> pInstance = GetInstance();

		std::shared_ptr<std::list<EventRegistrationPtr>> pRegistrations = pInstance->handlers[typeid(e)];

		// If thepRegistrationsregistrations list is null, then no handlers have been registered for this event
		if (pRegistrations == nullptr) {
			return;
		}

		// Iterate through all the registered handlers and dispatch to each one
		for (auto & reg : *(pRegistrations.get())) {
			// This is where some magic happens. The void * handler is statically cast to an event handler
			// of generic type Event and dispatched. The dispatch function will then do a dynamic
			// cast to the correct event type so the matching onEvent method can be called
			static_cast<EventHandler<Event>*>(reg->getHandler())->dispatch(e);
		}
	}


private:
	/**
	* \brief Default empty constructor
	*/
	EventBus() { }

	// Singleton class instance
	static std::shared_ptr<EventBus> ptrInstance;
	typedef std::unordered_map<std::type_index, std::shared_ptr<std::list<EventRegistrationPtr>>> TypeMap;
	TypeMap handlers;
};

#endif /* _SRC_EVENT_EVENT_BUS_H_ */
