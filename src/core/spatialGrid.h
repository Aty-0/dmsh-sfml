#pragma once 
#include "singleton.h"
#include "collider.h"

namespace dmsh::core 
{    
    class CollisionSpatialGrid : public Singleton<CollisionSpatialGrid>
    {
        using GridMatrix = std::vector<std::vector<std::shared_ptr<CollisionCell>>>; 
        public:
            CollisionSpatialGrid() = default;
            ~CollisionSpatialGrid();

            // Remove collider from cell by pointer 
            void removeColliderFromCell(std::shared_ptr<CollisionCell> cell, std::shared_ptr<Collider> collider);

            // Remove collider from cell by incices
            void removeColliderFromCell(const sf::Vector2i& indices, std::shared_ptr<Collider> collider);

            // Remove collider from all cells 
            void removeColliderFromCells(std::shared_ptr<Collider> collider);

            // Add collider by cells where he fits
            void addColliderToCells(std::shared_ptr<Collider> collider);

            // Get cell pointer by indices, where x it's a row and y it's a column
            inline std::shared_ptr<CollisionCell> getCell(const sf::Vector2i& indices) const { return m_cells[indices.x][indices.y]; }
            
            // Debug purpose only, draw grid
            // White cell it's a empty cell, red not empty  
            void onRender(sf::RenderWindow& window);

            // Check collisions on each cell
            void checkCollisions();
            
            // TODO: Replace to private and add method createCellsByScreen 
            // Create a grid of cells by count and size 
            void createCells(const sf::Vector2i& cells, const sf::Vector2f& size);

        private:
            GridMatrix m_cells;
    };
}