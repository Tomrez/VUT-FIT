using InformationSystem.Common.Tests.Seeds;
using InformationSystem.DAL;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Options;

namespace InformationSystem.Common.Tests;

public class InformationSystemTestingDbContext(DbContextOptions contextOptions, bool seedTestingData = false)
    : InformationSystemDbContext(contextOptions, seedDemoData: false)
{
    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        if (seedTestingData)
        {
            SubjectSeeds.Seed(modelBuilder);
            ActivitySeeds.Seed(modelBuilder);
            StudentSeeds.Seed(modelBuilder);
            StudentsInSubjectSeeds.Seed(modelBuilder);
            StudentEvaluationSeeds.Seed(modelBuilder);
        }
    }
}
