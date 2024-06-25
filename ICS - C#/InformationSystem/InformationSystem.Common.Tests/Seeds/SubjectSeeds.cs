using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;
using InformationSystem.DAL.Seeds;

namespace InformationSystem.Common.Tests.Seeds;

public static class SubjectSeeds
{
    public static readonly SubjectEntity EmptySubjectEntity = new()
    {
        Id = default,
        Name = default!,
        Abbreviation = default!
    };

    public static readonly SubjectEntity SubjectEntity1 = new()
    {
        Id = Guid.Parse("adfefb64-71e6-4d6f-8997-d48ff3949174"),
        Name = "Subject seeded subject 1",
        Abbreviation = "Subject seeded subject 1 abbreviation"
    };

    public static readonly SubjectEntity SubjectEntityWithNoStudentsAndNoActivities = SubjectEntity1 with { Id = Guid.Parse("08c8fe96-d6a3-48e5-b9eb-b33be3d1e439")};
    public static readonly SubjectEntity SubjectEntityUpdate = SubjectEntity1 with { Id = Guid.Parse("a67da1e7-ef21-4ad4-b8a3-7247a44e7ab2")};
    public static readonly SubjectEntity SubjectEntityDelete = SubjectEntity1 with { Id = Guid.Parse("9de91f95-b7af-4f52-b334-489bfbad8c2c")};

    public static readonly SubjectEntity StudentsInSubjectEntityUpdate = SubjectEntity1 with { Id = Guid.Parse("99f74467-c921-4a46-92d6-8012b760ba58")};
    public static readonly SubjectEntity StudentsInSubjectEntityDelete = SubjectEntity1 with { Id = Guid.Parse("d771d2d7-fbae-4f1f-8b48-dc3f24540b20")};


    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<SubjectEntity>().HasData(
            SubjectEntity1 with { Activities = Array.Empty<ActivityEntity>() },
            SubjectEntityWithNoStudentsAndNoActivities with { Activities = Array.Empty<ActivityEntity>() },
            SubjectEntityUpdate with { Activities = Array.Empty<ActivityEntity>() },
            SubjectEntityDelete with { Activities = Array.Empty<ActivityEntity>() },
            StudentsInSubjectEntityUpdate with { Activities = Array.Empty<ActivityEntity>() },
            StudentsInSubjectEntityDelete with { Activities = Array.Empty<ActivityEntity>() }
        );
    }
}
