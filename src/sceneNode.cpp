
#include "sceneNode.hpp"

SceneNode::SceneNode()
{
    parent = nullptr;
}

void SceneNode::attachChild(SceneNode::Ptr child)
{
    child->parent = this;
    children.push_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node)
{
    auto found = std::find_if(children.begin(), children.end(),
        [&] (SceneNode::Ptr& p) -> bool { return p.get() == &node; });

    assert(found != children.end());

    SceneNode::Ptr result = std::move(*found);
    result->parent = nullptr;
    children.erase(found);
    return result;
}

void SceneNode::draw(sf::RenderTarget& target,
                     sf::RenderStates states) const
{
    states.transform *= getTransform();

    drawCurrent(target, states);
/*
    for(auto itr = children.begin(); itr != children.end(); ++itr)
    {
        (*itr)->draw(target, states);
    }
*/

    std::for_each(children.begin(), children.end(),
        [&target, &states] (const SceneNode::Ptr& child) { child->draw(target, states); });
}

void SceneNode::drawCurrent(sf::RenderTarget& target,
                            sf::RenderStates states) const
{}

void SceneNode::update(sf::Time dt)
{
    updateCurrent(dt);
    updateChildren(dt);
}

void SceneNode::updateCurrent(sf::Time)
{}

void SceneNode::updateChildren(sf::Time dt)
{
    std::for_each(children.begin(), children.end(),
        [&dt] (SceneNode::Ptr& child) { child->update(dt); });
}

sf::Transform SceneNode::getWorldTransform() const
{
    sf::Transform transform = sf::Transform::Identity;
    for (const SceneNode* node = this; node != nullptr; node = node->parent)
        transform = node->getTransform() * transform;

    return transform;
}

sf::Vector2f SceneNode::getWorldPosition() const
{
    return getWorldTransform() * sf::Vector2f();
}
