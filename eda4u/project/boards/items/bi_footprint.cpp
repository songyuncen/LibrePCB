/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/

#include <QtCore>
#include "bi_footprint.h"
#include "bi_footprintpad.h"
#include "../board.h"
#include "../../project.h"
#include "../../circuit/circuit.h"
#include "../../library/projectlibrary.h"
#include <eda4ulibrary/fpt/footprint.h>
#include <eda4ulibrary/pkg/package.h>
#include <eda4ulibrary/cmp/component.h>
#include <eda4ucommon/fileio/xmldomelement.h>
#include <eda4ucommon/graphics/graphicsscene.h>
#include "../componentinstance.h"

namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

BI_Footprint::BI_Footprint(ComponentInstance& component, const XmlDomElement& domElement) throw (Exception) :
    BI_Base(), mComponentInstance(component), mFootprint(nullptr),
    mGraphicsItem(nullptr)
{
    mPosition.setX(domElement.getFirstChild("position", true)->getAttribute<Length>("x"));
    mPosition.setY(domElement.getFirstChild("position", true)->getAttribute<Length>("y"));
    mRotation = domElement.getFirstChild("position", true)->getAttribute<Angle>("rotation");

    init();
}

BI_Footprint::BI_Footprint(ComponentInstance& component, const Point& pos,
                           const Angle& rotation) throw (Exception) :
    BI_Base(), mComponentInstance(component), mFootprint(nullptr),
    mGraphicsItem(nullptr), mPosition(pos), mRotation(rotation)
{
    init();
}

void BI_Footprint::init() throw (Exception)
{
    QUuid footprintUuid = mComponentInstance.getLibPackage().getFootprintUuid();
    mFootprint = mComponentInstance.getBoard().getProject().getLibrary().getFootprint(footprintUuid);
    if (!mFootprint)
    {
        throw RuntimeError(__FILE__, __LINE__, footprintUuid.toString(),
            QString(tr("No footprint with the UUID \"%1\" found in the project's library."))
            .arg(footprintUuid.toString()));
    }

    mGraphicsItem = new BGI_Footprint(*this);
    mGraphicsItem->setPos(mPosition.toPxQPointF());
    mGraphicsItem->setRotation(mRotation.toDeg());

    const library::Component& libComp = mComponentInstance.getLibComponent();
    foreach (const library::FootprintPad* libPad, mFootprint->getPads())
    {
        BI_FootprintPad* pad = new BI_FootprintPad(*this, libPad->getUuid());
        if (mPads.contains(libPad->getUuid()))
        {
            throw RuntimeError(__FILE__, __LINE__, libPad->getUuid().toString(),
                QString(tr("The footprint pad UUID \"%1\" is defined multiple times."))
                .arg(libPad->getUuid().toString()));
        }
        if (!libComp.getPadSignalMap().contains(libPad->getUuid()))
        {
            throw RuntimeError(__FILE__, __LINE__, libPad->getUuid().toString(),
                QString(tr("Footprint pad \"%1\" not found in pad-signal-map of component \"%2\"."))
                .arg(libPad->getUuid().toString(), libComp.getUuid().toString()));
        }
        mPads.insert(libPad->getUuid(), pad);
    }
    if (mPads.count() != libComp.getPadSignalMap().count())
    {
        throw RuntimeError(__FILE__, __LINE__,
            QString("%1!=%2").arg(mPads.count()).arg(libComp.getPadSignalMap().count()),
            QString(tr("The pad count of the footprint \"%1\" does not match with "
            "the pad-signal-map of component \"%2\".")).arg(mFootprint->getUuid().toString(),
            libComp.getUuid().toString()));
    }

    // connect to the "attributes changed" signal of component instance
    connect(&mComponentInstance, &ComponentInstance::attributesChanged,
            this, &BI_Footprint::componentInstanceAttributesChanged);

    if (!checkAttributesValidity()) throw LogicError(__FILE__, __LINE__);
}

BI_Footprint::~BI_Footprint() noexcept
{
    qDeleteAll(mPads);              mPads.clear();
    delete mGraphicsItem;           mGraphicsItem = 0;
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

void BI_Footprint::addToBoard(GraphicsScene& scene) throw (Exception)
{
    scene.addItem(*mGraphicsItem);
    foreach (BI_FootprintPad* pad, mPads)
        pad->addToBoard(scene);
}

void BI_Footprint::removeFromBoard(GraphicsScene& scene) throw (Exception)
{
    scene.removeItem(*mGraphicsItem);
    foreach (BI_FootprintPad* pad, mPads)
        pad->removeFromBoard(scene);
}

XmlDomElement* BI_Footprint::serializeToXmlDomElement() const throw (Exception)
{
    if (!checkAttributesValidity()) throw LogicError(__FILE__, __LINE__);

    QScopedPointer<XmlDomElement> root(new XmlDomElement("footprint"));
    //root->setAttribute("uuid", mUuid);
    //root->setAttribute("gen_comp_instance", mGenCompInstance->getUuid());
    //root->setAttribute("symbol_item", mSymbVarItem->getUuid());
    XmlDomElement* position = root->appendChild("position");
    position->setAttribute("x", mPosition.getX());
    position->setAttribute("y", mPosition.getY());
    position->setAttribute("rotation", mRotation);
    return root.take();
}

/*****************************************************************************************
 *  Helper Methods
 ****************************************************************************************/

Point BI_Footprint::mapToScene(const Point& relativePos) const noexcept
{
    return (mPosition + relativePos).rotated(mRotation, mPosition);
}

bool BI_Footprint::getAttributeValue(const QString& attrNS, const QString& attrKey,
                                     bool passToParents, QString& value) const noexcept
{
    /*if ((attrNS == QLatin1String("SYM")) || (attrNS.isEmpty()))
    {
        if (attrKey == QLatin1String("NAME"))
            return value = getName(), true;
    }

    if ((attrNS != QLatin1String("SYM")) && (passToParents))
    {
        if (mGenCompInstance->getAttributeValue(attrNS, attrKey, false, value))
            return true;
        if (mSchematic.getAttributeValue(attrNS, attrKey, true, value))
            return true;
    }*/

    return false;
}

/*****************************************************************************************
 *  Inherited from SI_Base
 ****************************************************************************************/

QPainterPath BI_Footprint::getGrabAreaScenePx() const noexcept
{
    return mGraphicsItem->sceneTransform().map(mGraphicsItem->shape());
}

void BI_Footprint::setSelected(bool selected) noexcept
{
    BI_Base::setSelected(selected);
    mGraphicsItem->update();
    foreach (BI_FootprintPad* pad, mPads)
        pad->setSelected(selected);
}

/*****************************************************************************************
 *  Private Slots
 ****************************************************************************************/

void BI_Footprint::componentInstanceAttributesChanged()
{
    mGraphicsItem->updateCacheAndRepaint();
}

/*****************************************************************************************
 *  Private Methods
 ****************************************************************************************/

bool BI_Footprint::checkAttributesValidity() const noexcept
{
    if (mFootprint == nullptr)          return false;
    //if (mUuid.isNull())                 return false;
    //if (mGenCompInstance == nullptr)    return false;
    return true;
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
