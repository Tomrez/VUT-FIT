using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.Common.Tests.Seeds;

public static class StudentsInSubjectSeeds
{
    public static readonly StudentsInSubjectEntity EmptyStudentsInSubjectEntity = new()
    {
        Id = default,
        SubjectId = default,
        StudentId = default,
        Subject = default!,
        Student = default!
    };

    public static readonly StudentsInSubjectEntity StudentsInSubjectEntity1 = new()
    {
        Id = Guid.Parse("1f5fa005-1240-4d82-96fc-bd1be849bf29"),
        SubjectId = SubjectSeeds.SubjectEntity1.Id,
        StudentId = StudentSeeds.StudentEntity1.Id,
        Subject = SubjectSeeds.SubjectEntity1,
        Student = StudentSeeds.StudentEntity1
    };

    public static readonly StudentsInSubjectEntity StudentsInSubjectEntity2 = new()
    {
        Id = Guid.Parse("879b2eeb-ae64-4524-aa1a-f9471f4b3df1"),
        SubjectId = SubjectSeeds.SubjectEntity1.Id,
        StudentId = StudentSeeds.StudentEntity2.Id,
        Subject = SubjectSeeds.SubjectEntity1,
        Student = StudentSeeds.StudentEntity2
    };

    public static readonly StudentsInSubjectEntity StudentsInSubjectEntityUpdate = StudentsInSubjectEntity1 with { Id = Guid.Parse("cf30673e-ee3a-47fc-9a0a-088a150118d5"), Student = null!, Subject = null!, SubjectId = StudentSeeds.StudentEvaluationEntityUpdate.Id };
    public static readonly StudentsInSubjectEntity StudentsInSubjectEntityDelete = StudentsInSubjectEntity1 with { Id = Guid.Parse("8a1d38c0-ee37-486e-bd12-d378b56e2be9"), Student = null!, Subject = null!, SubjectId = StudentSeeds.StudentEvaluationEntityDelete.Id };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<StudentsInSubjectEntity>().HasData(
            StudentsInSubjectEntity1 with { Subject = null!, Student = null! },
            StudentsInSubjectEntity2 with { Subject = null!, Student = null! },
            StudentsInSubjectEntityUpdate,
            StudentsInSubjectEntityDelete
        );
    }
}
