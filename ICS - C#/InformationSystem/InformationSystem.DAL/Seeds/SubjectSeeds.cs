using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.Seeds;

public static class SubjectSeeds
{
    public static readonly SubjectEntity ICSSubject = new()
    {
        Id = Guid.Parse("fabde0cd-eefe-443f-baf6-3d96cc2cbf2e"),
        Name = "Seminar C#",
        Abbreviation = "ICS"
    };

    public static readonly SubjectEntity IPPSubject = new()
    {
        Id = Guid.Parse("f6b62a59-e951-4b88-ad69-273d28336145"),
        Name = "Principy programovacich jazyku a OPP",
        Abbreviation = "IPP"
    };

    public static readonly SubjectEntity IZUSubject = new()
    {
        Id = Guid.Parse("c895a221-2a33-453d-b79c-713886b391e7"),
        Name = "Zaklady umele inteligence",
        Abbreviation = "IZU"
    };

    public static readonly SubjectEntity IPKSubject = new()
    {
        Id = Guid.Parse("4fe4b566-9d9c-415d-9ca8-dd2b4d188a69"),
        Name = "Pocitacove komunikace a site",
        Abbreviation = "IPK"
    };

    static SubjectSeeds()
    {
        ICSSubject.Activities.Add(ActivitySeeds.MidtermActivityICS);
        ICSSubject.Activities.Add(ActivitySeeds.ExamActivityICS);
    }

    public static void Seed(this ModelBuilder modelBuilder) =>
        modelBuilder.Entity<SubjectEntity>().HasData(
            ICSSubject with { Activities = Array.Empty<ActivityEntity>() },
            IPPSubject with { Activities = Array.Empty<ActivityEntity>() },
            IZUSubject with { Activities = Array.Empty<ActivityEntity>() },
            IPKSubject with { Activities = Array.Empty<ActivityEntity>() }
        );
}