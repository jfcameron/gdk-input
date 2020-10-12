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

using namespace gdk;

//TODO: parameterize
const std::set<std::string> bindings{
	"Up",
	"Down",
	"Left",
	"Right",
	"A",
	"B",
};

/// \brief interactive controls configuration object
///
/// agnostic about presentation, so user must tie their own presentation
/// strategy into the the state of the object
///
/// vocab: 
///  binding: an association between a name and a number of inputs
///  name: a string passed to one of control's get methods. e.g control::get_down("name")
///  input: the name of a key, button, axis or hat on some human input device
///
/// proof of concept but I am extremely unhappy with the implementation here.
/// I want to throw away most of it
class configurator
{
public:
	class behaviour;

	std::shared_ptr<behaviour> m_Normal;

	std::shared_ptr<behaviour> m_pCurrent;

	std::shared_ptr<input::context> pInput;

	std::shared_ptr<controls> pControl;

	decltype(bindings.begin()) current_binding;

	enum class state
	{
		init,
		iterate_binding,
		new_binds_loop
	};

	/// \brief stateful behaviour interface
	class behaviour
	{
	public:
		virtual bool update() = 0;
	};

	/// \brief prompts user for a Y/N. 
	/// then performs a user defined Y or N behaviour
	class confirm : public behaviour
	{
		std::shared_ptr<input::context> pInput;

		std::function<void()> m_YesBehaviour;

		std::function<void()> m_NoBehaviour;

		virtual bool update() override
		{
			if (pInput->get_key_just_released(keyboard::Key::F1)) //likely parameterize the keys
			{
				m_YesBehaviour();
			}
			else if (pInput->get_key_just_released(keyboard::Key::F2))
			{
				m_NoBehaviour();
			}

			return false;
		}

	public:
		confirm(decltype(pInput) aInput,
			std::string aquestion,
			std::function<void()> aYesBehaviour,
			std::function<void()> aNoBehaviour)
			: pInput(aInput)
			, m_YesBehaviour(aYesBehaviour)
			, m_NoBehaviour(aNoBehaviour)
		{
			std::cout << aquestion << "\n";
		}
	};

	/// \brief normal state & behaviour of configurator
	///
	/// iterates binding names and all user inputs, waiting for
	/// the user to press something, at which point behaviour is swapped
	/// to a confirm instance
	class normal : public behaviour
	{
	public:
		configurator* pConfig;

		jfc::state_machine<state> sm = jfc::state_machine<state>(state::init);

		std::shared_ptr<input::context> pInput;

		virtual bool update() override
		{
			switch (sm.get())
			{
			case state::init:
			{
				std::cout << "init\n";

				pConfig->current_binding = bindings.begin();

				sm.set(state::iterate_binding);
			} break;

			case state::iterate_binding:
			{
				if (pConfig->current_binding != bindings.end())
				{
					sm.set(state::new_binds_loop);
				}
				else return true;
			} break;

			case state::new_binds_loop:
			{
				if (auto key = pInput->get_any_key_down())
				{
					pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
						"add key to current binding?",
						[&, key]()
						{
							pConfig->pControl->bind(*(pConfig->current_binding), key.value());

							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&]()
								{
									++pConfig->current_binding;

									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::iterate_binding);
								},
								[&]()
								{
									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::iterate_binding);
								}));
						},
						[&]() 
						{
							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&]()
								{
									++pConfig->current_binding;

									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::new_binds_loop);
								},
								[&]()
								{
									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::new_binds_loop);
								}));
						}));
				}

				// mouse buttons
				if (auto button = pInput->get_any_mouse_button_down())
				{
					pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
						"add mouse button to current binding?",
						[&, button]()
					{
						pConfig->pControl->bind(*(pConfig->current_binding), button.value());

						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
							"continue to next binding?",
							[&]()
						{
							++pConfig->current_binding;

							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::iterate_binding);
						},
							[&]()
						{
							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::iterate_binding);
						}));
					},
						[&]()
					{
						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
							"continue to next binding?",
							[&]()
						{
							++pConfig->current_binding;

							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::new_binds_loop);
						},
							[&]()
						{
							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::new_binds_loop);
						}));
					}));
				}

				// mouse axes
				/*if (auto axis = pInput->get_any_mouse_axis_down(0.9f))
				{
					pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
						"add mouse button to current binding?",
						[&, axis]()
					{
						pConfig->pControl->addMouseAxisToMapping(*(pConfig->current_binding), axis.value(), 0.1f);

						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
							"continue to next binding?",
							[&]()
						{
							++pConfig->current_binding;

							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::iterate_binding);
						},
							[&]()
						{
							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::iterate_binding);
						}));
					},
						[&]()
					{
						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
							"continue to next binding?",
							[&]()
						{
							++pConfig->current_binding;

							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::new_binds_loop);
						},
							[&]()
						{
							pConfig->m_pCurrent = pConfig->m_Normal;

							sm.set(state::new_binds_loop);
						}));
					}));
				}*/

				for (auto pGamepad : pInput->get_gamepads())
				{
					if (auto button = pGamepad->get_any_button_down())
					{
						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
						"add gamepad button to current binding?",
						[&, button, pGamepad]()
						{
							pConfig->pControl->bind(*(pConfig->current_binding), std::string(pGamepad->get_name()), button.value());

							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&, pGamepad]()
								{
									++pConfig->current_binding;

									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::iterate_binding);
								},
								[&]()
								{
									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::iterate_binding);
								}));
						},
						[&]() 
						{
							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&]()
								{
									++pConfig->current_binding;

									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::new_binds_loop);
								},
								[&]()
								{
									pConfig->m_pCurrent = pConfig->m_Normal;

									sm.set(state::new_binds_loop);
								}));
						}));
					}
					
					// gamepad hats
					if (auto hat = pGamepad->get_any_hat_down())
					{
						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
							"add gamepad hat to current binding?",
							[&, hat, pGamepad]()
						{
							pConfig->pControl->bind(*(pConfig->current_binding),
								std::string(pGamepad->get_name()), hat->first, hat->second);

							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&, pGamepad]()
							{
								++pConfig->current_binding;

								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::iterate_binding);
							},
								[&]()
							{
								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::iterate_binding);
							}));
						},
							[&]()
						{
							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&]()
							{
								++pConfig->current_binding;

								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::new_binds_loop);
							},
								[&]()
							{
								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::new_binds_loop);
							}));
						}));
					}

					// gamepad axes
					if (auto axis = pGamepad->get_any_axis_down(0.9f))
					{
						auto value = axis->second;
						value = std::floor(value / std::abs(value)) * 0.1f;
						//float minimum = value * 0.1f; //parameterize the minimum 0.1 here
						
						pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
							"add gamepad axis to current binding?",
							[&, axis, pGamepad, value]()
						{
							pConfig->pControl->bind(*(pConfig->current_binding),
								std::string(pGamepad->get_name()), axis->first, value);

							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&, pGamepad]()
							{
								++pConfig->current_binding;

								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::iterate_binding);
							},
								[&]()
							{
								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::iterate_binding);
							}));
						},
							[&]()
						{
							pConfig->m_pCurrent = decltype(m_pCurrent)(new confirm(pInput,
								"continue to next binding?",
								[&]()
							{
								++pConfig->current_binding;

								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::new_binds_loop);
							},
								[&]()
							{
								pConfig->m_pCurrent = pConfig->m_Normal;

								sm.set(state::new_binds_loop);
							}));
						}));
					}
				}
			} break;

			default: throw std::runtime_error("unhandled state");
			}

			return false;
		}

		std::shared_ptr<decltype(sm)::observer_type> observer;

	public:
		normal(decltype(pConfig) aConfig, decltype(pInput) aInput)
			: pConfig(aConfig)
			, pInput(aInput)
		{
			observer = std::make_shared<decltype(sm)::observer_type>([&](state o, state n)
			{
				if (n == state::new_binds_loop)
				{
					std::cout << "now binding: " << *(pConfig->current_binding) << "\n";
				}
			});

			sm.add_observer(observer);
		}
	};

public:
	bool update()
	{
		return m_pCurrent->update();
	}

	configurator(decltype(pInput) aInput, decltype(pControl) aControl)
		: m_Normal(new normal(this, aInput))
		, pControl(aControl)
	{
		m_pCurrent = m_Normal;
	}
};

bool bShouldExit = false;

int main(int argc, char **argv)
{
    auto pWindow = initGLFW();

	std::shared_ptr<input::context> pInput = std::move(input::context::make(pWindow.get()));

	auto pControl = std::shared_ptr<controls>(controls::make(pInput));

	configurator c(pInput, pControl);

	while (!bShouldExit)
	{
		glfwPollEvents();

		pInput->update();

		if (c.update()) bShouldExit = true;

		if (pInput->get_key_just_released(keyboard::Key::Escape)) bShouldExit = true;
	}

	std::cout << pControl->serialize_to_json() << "\n";

    return EXIT_SUCCESS;
}
