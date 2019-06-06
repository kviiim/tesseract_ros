/*
 * Copyright (c) 2009, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "rviz/display_context.h"
#include "rviz/ogre_helpers/arrow.h"
#include "rviz/ogre_helpers/shape.h"
#include "rviz/selection/selection_manager.h"

#include <tesseract_rviz/markers/arrow_marker.h>
#include <tesseract_rviz/markers/marker_selection_handler.h>

namespace tesseract_rviz
{

ArrowMarker::ArrowMarker( const std::string &ns,
                          const int id,
                          rviz::DisplayContext* context,
                          Ogre::SceneNode* parent_node)
  : MarkerBase(ns, id, context, parent_node)
  , arrow_( nullptr )
  , scale_(Ogre::Vector3(1,1,1))
{
  child_scene_node_ = scene_node_->createChildSceneNode();

  arrow_ = new rviz::Arrow(context_->getSceneManager(), child_scene_node_);
  setDefaultProportions();
  handler_.reset( new MarkerSelectionHandler( this, MarkerID(ns, id), context_ ));
  handler_->addTrackedObjects( arrow_->getSceneNode() );

  Ogre::Quaternion orient = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo( Ogre::Vector3(1,0,0) );
  arrow_->setOrientation( orient );
}

ArrowMarker::ArrowMarker( const std::string &ns,
                          const int id,
                          Ogre::Vector3 point1,
                          Ogre::Vector3 point2,
                          Ogre::Vector3 scale,
                          rviz::DisplayContext* context,
                          Ogre::SceneNode* parent_node)
  : MarkerBase(ns, id, context, parent_node)
  , arrow_( nullptr )
  , scale_(scale)
{
  child_scene_node_ = scene_node_->createChildSceneNode();

  arrow_ = new rviz::Arrow(context_->getSceneManager(), child_scene_node_);
  setDefaultProportions();
  handler_.reset( new MarkerSelectionHandler( this, MarkerID(ns, id), context_ ));
  handler_->addTrackedObjects( arrow_->getSceneNode() );

  Ogre::Vector3 direction = point2 - point1;
  float distance = direction.length();

  float head_length_proportion = 0.23f; // Seems to be a good value based on default in arrow.h of shaft:head ratio of 1:0.3
  float head_length = head_length_proportion*distance;
  if ( scale.z != 0.0f )
  {
    float length = scale.z;
    head_length = std::max<float>(0.0, std::min<float>(length, distance)); // clamp
  }
  float shaft_length = distance - head_length;

  arrow_->set(shaft_length, scale.x, head_length, scale.y);

  direction.normalise();

  // for some reason the arrow goes into the y direction by default
  Ogre::Quaternion orient = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo( direction );

  arrow_->setPosition(point1);
  arrow_->setOrientation( orient );
}

ArrowMarker::~ArrowMarker()
{
  delete arrow_;
  context_->getSceneManager()->destroySceneNode( child_scene_node_ );
}

void ArrowMarker::setDefaultProportions()
{
  arrow_->set(0.77, 1.0, 0.23, 2.0);
}

void ArrowMarker::setScale(Ogre::Vector3 scale)
{
  scale_ = scale_;
  arrow_->setScale(scale);
}

Ogre::Vector3 ArrowMarker::getScale() const
{
  return scale_;
}

void ArrowMarker::setColor( float r, float g, float b, float a )
{
  arrow_->setColor(r, g, b, a);
}

std::set<Ogre::MaterialPtr> ArrowMarker::getMaterials()
{
  std::set<Ogre::MaterialPtr> materials;
  extractMaterials( arrow_->getHead()->getEntity(), materials );
  extractMaterials( arrow_->getShaft()->getEntity(), materials );
  return materials;
}

}
