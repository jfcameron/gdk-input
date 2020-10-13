// © 2020 Joseph Cameron - All Rights Reserved

#include <demo/glfw_init.h>
#include <demo/state_machine.h> //This is copied from flappy. when connection back, remove this and split flappy impl to separate module

#include <gdk/input_context.h>
#include <gdk/controls.h>
#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>
#include <gdk/gamepad_glfw.h>

#include <cstdlib>
#include <vector>
#include <iostream>
#include <array>
#include <sstream>

using namespace gdk;

/////////////// HEADER ////////////////////////////////////////////////////////////////////////////

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
	jfc::state_machine<state> m_StateMachine = {state::select_current_binding};

	//! actions that can be taken on the current binding
	std::map<std::string, std::function<void(decltype(m_StateMachine)&)>> binding_actions = {
		{"show bindings", [](decltype(m_StateMachine) &a)
		{
			std::cout << "showing binding TODO!\n";
		}},
		{"add input", [](decltype(m_StateMachine)& a)
		{
			a.set(state::bind_new_input);
		}},
		{"clear inputs", [](decltype(m_StateMachine)& a)
		{
			a.set(state::clear_current_binding_of_inputs);
		}},
	};

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

/////////////// CPP    ////////////////////////////////////////////////////////////////////////////

configurator::configurator(decltype(m_pInput) aInput,
	decltype(m_pControl) aControl,
	const bindings_type& aBindings)
	: m_pInput(aInput)
	, m_pControl(aControl)
	, m_Bindings(aBindings)
	, m_Observer(std::make_shared<decltype(m_Observer)::element_type>([this](state o, state n)
{
	switch (n)
	{
	case state::select_current_binding:
	{
		std::cout << "select a binding to perform an action to\n";

		std::cout << *m_iCurrentBinding << "\n";
	} break;

	case state::choose_current_binding_action:
	{
		std::cout << "choose action to perform on " << (*m_iCurrentBinding) << "\n";

		std::cout << m_iCurrentAction->first << "\n";
	} break;

	case state::confirm_new_input:
	{
		std::cout << "confirm new input: " << m_LastInputString << "\n";
	} break;

	case state::clear_current_binding_of_inputs:
	{
		std::cout << "cleared all inputs from " << *m_iCurrentBinding << "\n";
	} break;

	case state::bind_new_input:
	{
		std::cout << "press any input to add it to the binding\n";
	} break;
	}
}))
{
	m_StateMachine.add_observer(m_Observer);

	m_StateMachine.set(state::select_current_binding);
}

void configurator::update()
{
	switch (m_StateMachine.get())
	{
	case state::select_current_binding:
	{
		if (m_pInput->get_key_just_released(keyboard::Key::UpArrow))
		{
			if (m_iCurrentBinding != m_Bindings.begin()) m_iCurrentBinding--;

			std::cout << *m_iCurrentBinding << "\n";
		}

		if (m_pInput->get_key_just_released(keyboard::Key::DownArrow))
		{
			if (m_iCurrentBinding != --m_Bindings.end()) m_iCurrentBinding++;

			std::cout << *m_iCurrentBinding << "\n";
		}

		if (m_pInput->get_key_just_released(keyboard::Key::Enter))
		{
			m_StateMachine.set(state::choose_current_binding_action);
		}
	} break;

	case state::choose_current_binding_action:
	{
		if (m_pInput->get_key_just_released(keyboard::Key::UpArrow))
		{
			if (m_iCurrentAction != binding_actions.begin()) m_iCurrentAction--;

			std::cout << m_iCurrentAction->first << "\n";
		}

		if (m_pInput->get_key_just_released(keyboard::Key::DownArrow))
		{
			if (m_iCurrentAction != --binding_actions.end()) m_iCurrentAction++;

			std::cout << m_iCurrentAction->first << "\n";
		}

		if (m_pInput->get_key_just_released(keyboard::Key::Enter))
		{
			m_iCurrentAction->second(m_StateMachine);
		}

		if (m_pInput->get_key_just_released(keyboard::Key::RightShift))
		{
			m_StateMachine.set(state::select_current_binding);
		}
	} break;

	case state::bind_new_input:
	{
		std::stringstream s;

		if (auto key = m_pInput->get_any_key_down())
		{
			m_ConfirmFunctor = [a = m_pControl, b = m_iCurrentBinding, c = key.value()]()
			{
				a->bind(*(b), c);
			};

			s << key.value();
			m_LastInputString = s.str();

			m_StateMachine.set(state::confirm_new_input);
		}

		if (auto button = m_pInput->get_any_mouse_button_down())
		{
			m_ConfirmFunctor = [=]()
			{
				m_pControl->bind(*(m_iCurrentBinding), button.value());
			};

			s << "Mouse " << button.value();
			m_LastInputString = s.str();

			m_StateMachine.set(state::confirm_new_input);
		}

		for (auto pGamepad : m_pInput->get_gamepads())
		{
			// gamepad buttons
			if (auto button = pGamepad->get_any_button_down())
			{
				m_ConfirmFunctor = [=]()
				{
					m_pControl->bind(*(m_iCurrentBinding),
						std::string(pGamepad->get_name()), button.value());
				};

				s << pGamepad->get_name() << " button " << button.value();
				m_LastInputString = s.str();

				m_StateMachine.set(state::confirm_new_input);
			}

			// gamepad hats
			if (auto hat = pGamepad->get_any_hat_down())
			{
				m_ConfirmFunctor = [=]()
				{
					m_pControl->bind(*(m_iCurrentBinding),
						std::string(pGamepad->get_name()), hat->first, hat->second);
				};

				s << pGamepad->get_name() << " hat " << hat->first;
				m_LastInputString = s.str();

				m_StateMachine.set(state::confirm_new_input);
			}

			// gamepad axes
			if (auto axis = pGamepad->get_any_axis_down(0.9f))
			{				
				m_ConfirmFunctor = [=]()
				{
					auto value = axis->second;
					value = std::floor(value / std::abs(value)) * 0.1f;
					//float minimum = value * 0.1f; //parameterize the minimum 0.1 here


					m_pControl->bind(*(m_iCurrentBinding),
						std::string(pGamepad->get_name()), axis->first, value);
				};

				s << pGamepad->get_name() << " axis " << std::to_string(axis->first);
				m_LastInputString = s.str();

				m_StateMachine.set(state::confirm_new_input);
			}
		}

		// mouse axes
		/*if (auto axis = pInput->get_any_mouse_axis_down(0.9f))
		{
			pConfig->pControl->addMouseAxisToMapping(*(pConfig->current_binding), axis.value(), 0.1f);
		}*/
	} break;

	case state::confirm_new_input:
	{
		if (m_pInput->get_key_just_released(keyboard::Key::Enter))
		{
			m_ConfirmFunctor();

			std::cout << "input added\n";

			m_StateMachine.set(state::choose_current_binding_action);
		}

		if (m_pInput->get_key_just_released(keyboard::Key::RightShift))
		{
			std::cout << "input cancelled\n";

			m_StateMachine.set(state::choose_current_binding_action);
		}
	} break;

	case state::clear_current_binding_of_inputs:
	{
		m_pControl->unbind(*m_iCurrentBinding);

		m_StateMachine.set(state::choose_current_binding_action);
	} break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    auto pWindow = initGLFW();

	std::shared_ptr<input::context> pInput = std::move(input::context::make(pWindow.get()));

	auto pControl = std::shared_ptr<controls>(controls::make(pInput));

	configurator c(pInput, pControl, {
		"Up",
		"Down",
		"Left",
		"Right",
		"A",
		"B",
	});

	for (bool bShouldExit(false);!bShouldExit;)
	{
		glfwPollEvents();

		pInput->update();

		c.update();

		if (pInput->get_key_just_released(keyboard::Key::Escape)) bShouldExit = true;
	}

	std::cout << pControl->serialize_to_json() << "\n";

    return EXIT_SUCCESS;
}
