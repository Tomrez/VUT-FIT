using InformationSystem.DAL.Entities;

namespace InformationSystem.DAL.Mappers;

public class StudentInSubjectEntityMapper : IEntityMapper<StudentsInSubjectEntity>
{
    public void MapToExistingEntity(StudentsInSubjectEntity existingEntity, StudentsInSubjectEntity newEntity)
    {
        existingEntity.StudentId = newEntity.StudentId;
        existingEntity.SubjectId = newEntity.SubjectId;
    }
}