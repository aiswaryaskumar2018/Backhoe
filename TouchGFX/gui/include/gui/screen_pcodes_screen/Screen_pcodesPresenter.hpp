#ifndef SCREEN_PCODESPRESENTER_HPP
#define SCREEN_PCODESPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen_pcodesView;

class Screen_pcodesPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen_pcodesPresenter(Screen_pcodesView& v);

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

    virtual ~Screen_pcodesPresenter() {}

private:
    Screen_pcodesPresenter();

    Screen_pcodesView& view;
};

#endif // SCREEN_PCODESPRESENTER_HPP
