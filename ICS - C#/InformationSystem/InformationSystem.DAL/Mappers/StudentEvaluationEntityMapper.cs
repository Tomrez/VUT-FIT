using InformationSystem.DAL.Entities;

namespace InformationSystem.DAL.Mappers;

public class StudentEvaluationEntityMapper : IEntityMapper<StudentEvaluationEntity>
{
    public void MapToExistingEntity(StudentEvaluationEntity existingEntity, StudentEvaluationEntity newEntity)
    {
        existingEntity.StudentId = newEntity.StudentId;
        existingEntity.ActivityId = newEntity.ActivityId;
        existingEntity.Points = newEntity.Points;
        existingEntity.Notes = newEntity.Notes;
        existingEntity.Evaluator = newEntity.Evaluator;
    }
}