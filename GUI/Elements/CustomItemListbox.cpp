#include "CustomItemListbox.h"
#include <algorithm>

namespace CEGUI
{
	String CustomItemListbox::WidgetTypeName = "CustomItemListbox";

	//----------------------------------------------------------------------------//
	CustomItemListbox::CustomItemListbox(const String& type, const String& name) :
		// initialise base class
		ItemListbox(type, name)
	{
	}

	/************************************************************************
		Layout items
	************************************************************************/
	void CustomItemListbox::layoutItemWidgets()
	{
		float y = 0;
		float widest = 0;

		ItemEntryList::iterator i = d_listItems.begin();
		ItemEntryList::iterator end = d_listItems.end();

		while (i!=end)
		{
			ItemEntry* entry = *i;
			const Size pxs = entry->getItemPixelSize();
			if (pxs.d_width > widest)
			{
				widest = pxs.d_width;
			}

			entry->setArea(URect(
				UVector2(cegui_absdim(0), cegui_absdim(y)),
				UVector2(cegui_reldim(1), cegui_absdim(y+pxs.d_height))
				));

			y+=pxs.d_height;

			++i;
		}

		// reconfigure scrollbars
		configureScrollbars(Size(widest,y));
	}

	/************************************************************************
		Configure scroll bars
	************************************************************************/
	void CustomItemListbox::configureScrollbars(const Size& doc_size)
	{
		Scrollbar* v = getVertScrollbar();
		Scrollbar* h = getHorzScrollbar();

		const bool old_vert_visible = v->isVisible(true);
		const bool old_horz_visible = h->isVisible(true);

		Size render_area_size = getItemRenderArea().getSize();

		// setup the pane size
		float pane_size_w = ceguimax(doc_size.d_width, render_area_size.d_width);
		UVector2 pane_size(cegui_absdim(pane_size_w), cegui_absdim(doc_size.d_height));

		d_pane->setMinSize(pane_size);
		d_pane->setMaxSize(pane_size);
		//d_pane->setWindowSize(pane_size);

		// "fix" scrollbar visibility
		if (d_forceVScroll || doc_size.d_height > render_area_size.d_height)
		{
			v->show();
		}
		else
		{
			v->hide();
		}

		//render_area_size = getItemRenderArea().getSize();

		if (d_forceHScroll || doc_size.d_width > render_area_size.d_width)
		{
			h->show();
		}
		else
		{
			h->hide();
		}

		// if some change occurred, invalidate the inner rect area caches.
		if ((old_vert_visible != v->isVisible(true)) ||
			(old_horz_visible != h->isVisible(true)))
		{
			d_innerUnclippedRectValid = false;
			d_innerRectClipperValid = false;
		}

		// get a fresh item render area
		Rect render_area = getItemRenderArea();
		render_area_size = render_area.getSize();

		// update the pane clipper area
		static_cast<ClippedContainer*>(d_pane)->setClipArea(render_area);

		// get height of an item and use it as the height for scroll bar step size.
		float item_height = 0;
		auto i = d_listItems.begin();
		if (i != d_listItems.end())
		{
			item_height = (*i)->getItemPixelSize().d_height;
		}

		// setup vertical scrollbar
		v->setDocumentSize(doc_size.d_height);
		v->setPageSize(render_area_size.d_height);
		v->setStepSize(ceguimax(1.0f, item_height));
		v->setScrollPosition(v->getScrollPosition());

		v->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&CustomItemListbox::handle_VScroll,this));

		// setup horizontal scrollbar
		h->setDocumentSize(doc_size.d_width);
		h->setPageSize(render_area_size.d_width);
		h->setStepSize(ceguimax(1.0f, item_height));
		h->setScrollPosition(h->getScrollPosition());
	}

	/************************************************************************
		Event subscribers for scrolling
	************************************************************************/
	bool CustomItemListbox::handle_VScroll(const EventArgs& e)
	{
		// The list contains items of the same height so scrolling it by the step size.
		// The step size has been set to the item height.
		const WindowEventArgs& we = static_cast<const WindowEventArgs&>(e);
		Scrollbar* v = static_cast<Scrollbar*>(we.window);
		float newpos = -static_cast<int>(v->getScrollPosition() / v->getStepSize()) * v->getStepSize();
		d_pane->setYPosition(cegui_absdim(newpos));
		return true;
	}

	/*************************************************************************
		Handler called internally when the list contents are changed
	*************************************************************************/
	void CustomItemListbox::onListContentsChanged(WindowEventArgs& e)
	{
		// if we are not currently initialising we might have things todo
		if (!d_initialising)
		{
			invalidate();

			// if auto resize is enabled - do it
			if (d_autoResize)
				sizeToContent();

			// resort list if requested and enabled
			if (d_resort && d_sortEnabled)
				sortList(false);
			d_resort = false;

			// redo the item layout and fire our event
			performChildWindowLayout();
			fireEvent(EventListContentsChanged, e, EventNamespace);
		}
	}

	/************************************************************************
		Sort list
	************************************************************************/
	void CustomItemListbox::sortList(bool relayout)
	{
		std::sort(d_listItems.begin(), d_listItems.end(), getRealSortCallback());
		if (relayout)
		{
			performChildWindowLayout();
		}
	}
}