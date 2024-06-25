using InformationSystem.DAL.Entities;

namespace InformationSystem.DAL.Mappers;

public interface IEntityMapper<in TEntity> 
    where TEntity : IEntity
{
    void MapToExistingEntity(TEntity existingEntity, TEntity newEntity);
}