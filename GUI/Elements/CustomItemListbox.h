#ifndef GUI_ELEMENTS_SCROLL_ITEM_LISTBOX_H
#define GUI_ELEMENTS_SCROLL_ITEM_LISTBOX_H

#include <CEGUI/CEGUI.h>

namespace CEGUI
{
// Only to be used when the ItemListbox contains only items of the same height.
//
//! A ItemListbox based class that changes content pane y position by the step size of the attached vertical scroll bar.
// Making sure that items are rendered fully visible and not just a portion of them.
// The step size is set to the height of an item.
class CustomItemListbox : public ItemListbox
{
public:
	static CEGUI::String WidgetTypeName; //!< Window factory name

    //! Constructor
    CustomItemListbox(const String& type, const String& name);

    //! Destructor
    virtual ~CustomItemListbox(void) {}

	virtual void layoutItemWidgets();

    /*!
    \brief
        Sort the list.

    \param relayout
        True if the item layout should be redone after the sorting.
        False to only sort the internal list. Nothing more.

        This parameter defaults to true and should generally not be
        used in client code.
    */
    void sortList(bool relayout=true);

protected:
    /*!
    \brief
        Configure scrollbars
    */
    void configureScrollbars(const Size& doc_size);

	/*!
	\brief
		Handler called internally when the list contents are changed
	*/
	virtual	void	onListContentsChanged(WindowEventArgs& e);

    /************************************************************************
        Event subscribers
    ************************************************************************/
	bool handle_VScroll(const EventArgs& e);
};

}

#endif // GUI_ELEMENTS_SCROLL_ITEM_LISTBOX_H