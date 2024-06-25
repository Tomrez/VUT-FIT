using InformationSystem.DAL.Entities;

namespace InformationSystem.DAL.Mappers;

public class EntityBaseMapper : IEntityMapper<EntityBase>
{
    public void MapToExistingEntity(EntityBase existingEntity, EntityBase newEntity)
    {
        existingEntity.Id = newEntity.Id;
    }
}