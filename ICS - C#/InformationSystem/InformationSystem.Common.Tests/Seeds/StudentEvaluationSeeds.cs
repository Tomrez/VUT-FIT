using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;
using Microsoft.VisualStudio.TestPlatform.ObjectModel;

namespace InformationSystem.Common.Tests.Seeds;

public static class StudentEvaluationSeeds
{
    public static readonly StudentEvaluationEntity EmptyStudentEvaluationEntity = new()
    {
        Id = default,
        StudentId = default,
        ActivityId = default,
        Points = default!,
        Notes = default,
        Evaluator = default,
        Student = default!,
        Activity = default!
    };

    public static readonly StudentEvaluationEntity StudentEvaluationEntity1 = new()
    {
        Id = Guid.Parse("21e0c88d-1833-4930-ae14-3947e0a027f6"),
        StudentId = StudentSeeds.StudentEntity1.Id,
        ActivityId = ActivitySeeds.ActivityEntity1.Id,
        Points = 42,
        Notes = "StudentEvaluation seeded student 1 activity 1 notes",
        Evaluator = "StudentEvaluation seeded student 1 activity 1 evaluator",
        Student = StudentSeeds.StudentEntity1,
        Activity = ActivitySeeds.ActivityEntity1
    };

    public static readonly StudentEvaluationEntity StudentEvaluationEntityUpdate = StudentEvaluationEntity1 with { Id = Guid.Parse("ef7c5014-3a04-4e99-bd36-1e8cfaa3c07d"), Student = null!, Activity = null!, StudentId = StudentSeeds.StudentEvaluationEntityUpdate.Id };
    public static readonly StudentEvaluationEntity StudentEvaluationEntityDelete = StudentEvaluationEntity1 with { Id = Guid.Parse("3057f3dc-5e14-4b94-bd22-0e6c104b9e7b"), Student = null!, Activity = null!, StudentId = StudentSeeds.StudentEvaluationEntityDelete.Id };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<StudentEvaluationEntity>().HasData(
            StudentEvaluationEntity1 with { Student = null!, Activity = null! },
            StudentEvaluationEntityUpdate,
            StudentEvaluationEntityDelete
        );
    }
}
