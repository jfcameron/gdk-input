// © 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_CONFIGURATOR_H
#define GDK_CONFIGURATOR_H

#include <set>
#include <map>
#include <functional>

#include <gdk/input_context.h>
#include <gdk/controls.h>

//This is copied from flappy. when connection back, remove this and split flappy impl to separate module
#include <jfc/state_machine.h> 

namespace gdk
{
	/// \brief interactive control configuration dialogue
	///
	/// agnostic about presentation. user must provide functors to present dialogue state
	/// in their GUI
	class configurator final
	{
	public:
		using bindings_type = std::set<std::string>;

		//! state of the configurator
		enum class state
		{
			select_current_binding, //!< select a binding to perform an action on
			choose_current_binding_action, //!< choose an action to perform on the selected binding
			bind_new_input, //!< awaiting user input to bind
			confirm_new_input, //!< add input if user confirms
			clear_current_binding_of_inputs, //!< removes inputs from current binding
		};

	private:
		//! state of this object
		jfc::state_machine<state> m_StateMachine = { state::select_current_binding };

		//! actions that can be taken on the current binding
		std::map<std::string, std::function<void(decltype(m_StateMachine)&)>> binding_actions;

		//! ptr to the input context
		std::shared_ptr<input::context> m_pInput;

		//! ptr to the control that will be modified
		std::shared_ptr<controls> m_pControl;

		//! bindings provided at ctor time
		bindings_type m_Bindings;

		//! iterator to the current binding
		decltype(m_Bindings)::iterator m_iCurrentBinding = m_Bindings.begin();

		//! iterator to the current binding action
		std::remove_const<decltype(binding_actions)::iterator>::type m_iCurrentAction = binding_actions.begin();

		std::shared_ptr<decltype(m_StateMachine)::observer_type> m_Observer;

		//! functor called during confirm state
		std::function<void()> m_ConfirmFunctor;

		//! String representation of the last input
		std::string m_LastInputString;

	public:
		//! must be called in an update loop
		void update();

		//! deleting move semantics
		configurator& operator=(configurator&&) = delete;
		//! deleting move semantics
		configurator(configurator&&) = delete;

		//! deleting copy semantics
		configurator& operator=(const configurator&) = delete;
		//! deleting copy semantics
		configurator(const configurator&) = delete;

		//! ctor
		configurator(decltype(m_pInput) aInput,
			decltype(m_pControl) aControl,
			const bindings_type& aBindings);
	};
}

#endif
