#ifndef SCREEN_MENUPRESENTER_HPP
#define SCREEN_MENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen_menuView;

class Screen_menuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen_menuPresenter(Screen_menuView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~Screen_menuPresenter() {}

private:
    Screen_menuPresenter();

    Screen_menuView& view;
};

#endif // SCREEN_MENUPRESENTER_HPP
