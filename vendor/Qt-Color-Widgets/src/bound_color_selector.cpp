/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2012-2015 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "bound_color_selector.hpp"

namespace color_widgets {

BoundColorSelector::BoundColorSelector(QColor* reference, QWidget *parent) :
    ColorSelector(parent), ref(reference)
{
    setColor(*reference);
    connect(this,SIGNAL(colorChanged(QColor)),SLOT(update_reference(QColor)));
}

void BoundColorSelector::update_reference(QColor c)
{
    *ref = c;
}

} // namespace color_widgets
