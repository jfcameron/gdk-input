// © 2020 Joseph Cameron - All Rights Reserved

#include <gdk/configurator.h>

#include <iostream>
#include <sstream>

using namespace gdk;

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
, binding_actions({
	{"show bindings", [](decltype(m_StateMachine)& a)
	{
		a.set(state::display_bindings);
	}},
	{"add input", [](decltype(m_StateMachine)& a)
	{
		a.set(state::bind_new_input);
	}},
	{"clear inputs", [](decltype(m_StateMachine)& a)
	{
		a.set(state::clear_current_binding_of_inputs);
	}},
})
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
			if (auto axis = pGamepad->get_any_axis_down(0.9f)) //parameterize 0.9
			{
				m_ConfirmFunctor = [=]()
				{
					auto value = axis->second;
					value = std::floor(value / std::abs(value)) * 0.1f; //parameterize 0.1
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

	case state::display_bindings:
	{
		std::cout << "displaying bindings\n";

		auto bindings = m_pControl->get_bindings(*m_iCurrentBinding);

		std::cout << "display the bindings somehow or whatever\n";

		m_StateMachine.set(state::choose_current_binding_action);
	} break;
	}
}
