#include "spatialGrid.h"
#include "debug.h"
#include "gameObject.h"
#include "resourceManager.h"

namespace dmsh::core
{
    CollisionSpatialGrid::~CollisionSpatialGrid()
    {
        m_cells.clear();
        m_cells.shrink_to_fit();
    }

    void CollisionSpatialGrid::createCells(const sf::Vector2i& cells, const sf::Vector2f& size)
    {        
        m_cells.reserve(cells.x);

        for (std::int32_t row = 0; row < cells.x; ++row)
        {
            std::vector<std::shared_ptr<CollisionCell>> vCol = { };
            for (std::int32_t col = 0; col < cells.y; ++col)
            {
                auto cell = std::make_shared<CollisionCell>();
                cell->Bounds = { {row * size.x, col * size.y }, size };                                
                cell->Indices = { row, col };
                vCol.push_back(std::move(cell));
            }            

            m_cells.push_back(std::move(vCol));
        }
    }
    
    void CollisionSpatialGrid::removeColliderFromCell(std::shared_ptr<CollisionCell> cell, std::shared_ptr<Collider> collider)
    {
        if (cell == nullptr)
        {
            return;
        }
        
        auto it = std::find_if(cell->Colliders.begin(), cell->Colliders.end(), [&collider](auto cellColider) {
            return collider == cellColider;
        });

        if (it != cell->Colliders.end())
        {
            //DMSH_DEBUG("remove collider from cell %i %i | %s %i", cell->Indices.x, cell->Indices.y, collider->getOwner()->getTag().c_str(), collider->getOwner()->getId());
            cell->Colliders.erase(it);
            collider->m_cells.erase(cell);
        }
    }

    void CollisionSpatialGrid::removeColliderFromCell(const sf::Vector2i& indices, std::shared_ptr<Collider> collider)
    {
        auto cell = m_cells[indices.x][indices.y];
        removeColliderFromCell(cell, collider);
    }

    void CollisionSpatialGrid::removeColliderFromCells(std::shared_ptr<Collider> collider)
    {
        for (auto row : m_cells)
        {
            for (auto& col : row)
            {          
                if (col->Colliders.size() == 0)
                {
                    continue;
                }

                removeColliderFromCell(col, collider);
            }
        }
    }

    void CollisionSpatialGrid::addColliderToCells(std::shared_ptr<Collider> collider)
    {
        for (const auto row : m_cells)
        {
            for (const auto& col : row)
            {       
                // Do not add statics colliders
                if (collider->isStatic())
                    continue;

                if (collider->intersect(col->Bounds))
                {
                    // Add active cell to collider 
                    collider->m_cells.insert(col);
                    
                    // TODO: Meh
                    auto it = std::find_if(col->Colliders.begin(), col->Colliders.end(), [&collider](auto cellColider) {
                        return collider == cellColider;
                    });

                    if (it == col->Colliders.end())
                    {
                        //DMSH_DEBUG("add collider to cell %i %i", col->Indices.x, col->Indices.y);

                        // Add collider to cell's collider list 
                        col->Colliders.push_back(collider);
                    }
                }
                else 
                {
                    // Else maybe collider's pointer can stay in cell but rn it's not on cell
                    // And we can remove him 
                    removeColliderFromCell(col, collider);
                }
            }
        }
    }

    void CollisionSpatialGrid::checkCollisions()
    {
        for (const auto row : m_cells)
        {
            for (const auto& cell : row)
            {  
                if (cell->Colliders.size() <= 1)
                {
                    continue;
                }

                for (std::int32_t i = 0; i < cell->Colliders.size(); ++i)
                {
                    auto& A = cell->Colliders[i];
                    if (!A || A->isStatic()) 
                        continue;
                    
                    auto AGo = A->getOwner();
                    if (!AGo->isVisible()) 
                        continue;

                    for (std::int32_t j = i + 1; j < cell->Colliders.size(); ++j)
                    {
                        auto& B = cell->Colliders[j];

                        if (!B || B->isStatic()) 
                            continue;

                        auto BGo = B->getOwner();

                        if (!BGo->isVisible()) 
                            continue;

                        if (A->intersect(*B))
                        {
                            // Try to find a game object pointer on a prev frame, if we are not found a go, 
                            // then we are invoke a start callback
                            if (!A->m_collisionTracker.isObjectCollidedPrevFrame(B))
                            {
                                //DMSH_DEBUG("it's a on collision enter callback tag:%s id:%d -> tag:%s id:%d", AGo->getTag().c_str(), AGo->getId(), BGo->getTag().c_str(), BGo->getId());
                                AGo->onCollisionEnter(*B);
                            }

                            //DMSH_DEBUG("stay tag:%s id:%d -> tag:%s id:%d", AGo->getTag().c_str(), AGo->getId(), BGo->getTag().c_str(), BGo->getId());
                            // Invoke a stay collision callback
                            AGo->onCollisionStay(*B);
                            A->m_collisionTracker.addCollidedObject(B);
                        }   

                        for (auto other : A->m_collisionTracker.getCollidedInPrevFrame()) 
                        {
                            if (!A->m_collisionTracker.isObjectCollided(B)) 
                            {
                                // TODO: other->getCollider() it's a wrong I think
                                //DMSH_DEBUG("it's a on collision exit callback tag:%s id:%d -> tag:%s id:%d", AGo->getTag().c_str(), AGo->getId(), BGo->getTag().c_str(), BGo->getId());
                                BGo->onCollisionExit(*A);
                            }
                        }  

                        A->m_collisionTracker.updateFrame(); 
                    }  
                }
            }
        } 
    }   

#define DEBUG_COLLISION_GRID_SHOW_TEXT
    void CollisionSpatialGrid::onRender(sf::RenderTarget& window)
    {
        for (const auto row : m_cells)
        {
            for (const auto cell : row)
            {
                if (!cell) 
                {
                    // TODO: What's need todo in that case ?
                    continue;
                }

                const auto color = cell->Colliders.size() == 0 ? sf::Color::White : sf::Color::Red;
                debug::Square::draw(window, static_cast<sf::Vector2f>(cell->Bounds.position), cell->Bounds.size * 0.95f, color);
#ifdef DEBUG_COLLISION_GRID_SHOW_TEXT
                if (cell->Colliders.size() != 0)
                {
                    static const auto font = ResourceManager::getInstance()->get<ResourceTypes::Font>("immortal");

                    sf::Text text(*font->getHandle());
                    text.setPosition(cell->Bounds.position);                    
                    text.setCharacterSize(10);
                    text.setFillColor(sf::Color::Cyan);

                    std::stringstream stream;
                    for (auto collider : cell->Colliders)
                    {
                        auto owner = collider->getOwner();
                        stream << "id:" << owner->getId() << " tag:" << owner->getTag() << "\n";
                    }
                    
                    text.setString(stream.str());
                    
                    window.draw(text);
                    stream.clear();
                }
#endif
            }
        }
    }
}