using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.Common.Tests.Seeds;

public static class StudentSeeds
{
    public static readonly StudentEntity EmptyStudent = new()
    {
        Id = default,
        Login = default!,
        Name = default!,
        Surname = default!,
        PhotoUrl = default
    };

    public static readonly StudentEntity StudentEntity1 = new()
    {
        Id = Guid.Parse("c4b94cbb-0b18-49b3-9e1e-4cdff4c235cb"),
        Login = "Student seeded student 1 login",
        Name = "Student seeded student 1 name",
        Surname = "Student seeded student 1 surname",
        PhotoUrl = null
    };

    public static readonly StudentEntity StudentEntityUpdate = StudentEntity1 with { Id = Guid.Parse("ae194720-040b-4642-bbc4-17d47082a535"), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() };
    public static readonly StudentEntity StudentEntityDelete = StudentEntity1 with { Id = Guid.Parse("c9e95fcf-5d9d-45a7-b772-ae5e18de3fae"), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() };

    public static readonly StudentEntity StudentEvaluationEntityUpdate = StudentEntity1 with { Id = Guid.Parse("b42a5b7d-7d85-4ff1-8f67-e42edf215a9a"), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() };
    public static readonly StudentEntity StudentEvaluationEntityDelete = StudentEntity1 with { Id = Guid.Parse("a5c9ebe9-439f-45ee-a138-f61bdcdb1a89"), ActivityEvaluation = new List<StudentEvaluationEntity>() };


    public static StudentEntity StudentEntity2 = new()
    {
        Id = Guid.Parse("defe63de-3be7-44a1-b1f2-1fba2a62b73e"),
        Login = "Student seeded student 2 login",
        Name = "Student seeded student 2 name",
        Surname = "Student seeded student 2 surname",
        PhotoUrl = null
    };

    static StudentSeeds()
    {
        StudentEntity1.ActivityEvaluation.Add(StudentEvaluationSeeds.StudentEvaluationEntity1);
        StudentEvaluationEntityDelete.ActivityEvaluation.Add(StudentEvaluationSeeds.StudentEvaluationEntityDelete);

        StudentEntity1.Subjects.Add(StudentsInSubjectSeeds.StudentsInSubjectEntity1);
        StudentEntity1.Subjects.Add(StudentsInSubjectSeeds.StudentsInSubjectEntity2);
        StudentEvaluationEntityDelete.Subjects.Add(StudentsInSubjectSeeds.StudentsInSubjectEntityDelete);
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<StudentEntity>().HasData(
            StudentEntity1 with { ActivityEvaluation = Array.Empty<StudentEvaluationEntity>(), Subjects = Array.Empty<StudentsInSubjectEntity>() },
            StudentEntity2 with { ActivityEvaluation = Array.Empty<StudentEvaluationEntity>(), Subjects = Array.Empty<StudentsInSubjectEntity>() },
            StudentEntityUpdate with { ActivityEvaluation = Array.Empty<StudentEvaluationEntity>(), Subjects = Array.Empty<StudentsInSubjectEntity>() },
            StudentEntityDelete with { ActivityEvaluation = Array.Empty<StudentEvaluationEntity>(), Subjects = Array.Empty<StudentsInSubjectEntity>() },
            StudentEvaluationEntityUpdate with { ActivityEvaluation = Array.Empty<StudentEvaluationEntity>(), Subjects = Array.Empty<StudentsInSubjectEntity>() },
            StudentEvaluationEntityDelete with { ActivityEvaluation = Array.Empty<StudentEvaluationEntity>(), Subjects = Array.Empty<StudentsInSubjectEntity>() }
        );
    }
}
