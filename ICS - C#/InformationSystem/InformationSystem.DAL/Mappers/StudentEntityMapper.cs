using InformationSystem.DAL.Entities;

namespace InformationSystem.DAL.Mappers;

public class StudentEntityMapper : IEntityMapper<StudentEntity>
{   public void MapToExistingEntity(StudentEntity existingEntity, StudentEntity newEntity)
    {
        existingEntity.Login = newEntity.Login;
        existingEntity.Name = newEntity.Name;
        existingEntity.Surname = newEntity.Surname;
        existingEntity.PhotoUrl = newEntity.PhotoUrl;
    }
}