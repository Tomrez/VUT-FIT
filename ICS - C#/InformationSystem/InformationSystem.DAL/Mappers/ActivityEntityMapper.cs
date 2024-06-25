using InformationSystem.DAL.Entities;

namespace InformationSystem.DAL.Mappers;

public class ActivityEntityMapper : IEntityMapper<ActivityEntity>
{
    public void MapToExistingEntity(ActivityEntity existingEntity, ActivityEntity newEntity)
    {
        existingEntity.Start = newEntity.Start;
        existingEntity.End = newEntity.End;
        existingEntity.Room = newEntity.Room;
        existingEntity.ActivityType = newEntity.ActivityType;
        existingEntity.Description = newEntity.Description;
        existingEntity.SubjectId = newEntity.SubjectId;
    }
}