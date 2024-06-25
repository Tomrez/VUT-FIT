using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Seeds;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Tests;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Internal;
using Microsoft.VisualStudio.TestPlatform.CommunicationUtilities.ObjectModel;
using Xunit;
using Xunit.Abstractions;
using SubjectSeeds = InformationSystem.DAL.Seeds.SubjectSeeds;


namespace InformationSystem.DAL.Tests;

public class DbContextActivityTests(ITestOutputHelper output) : DbContextTestsBase(output)
{

    [Fact]
    public async Task AddNew_ActivityInSubject_Persisted()
    {
        //Arrange
        var entity = ActivitySeeds.EmptyActivity with
        {
            Start = new DateTime(2024, 04,04),
            End = new DateTime(2024,05,05),
            Room = Room.D105,
            ActivityType = ActivityType.FinalExam,
            Description = "Zaverecna zkouska",
            SubjectId = SubjectSeeds.ICSSubject.Id,
            Subject = SubjectSeeds.ICSSubject
        };
        
        //Act
        InformationSystemDbContextSUT.Activities.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();
        
        //Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Activities
            .Include(a => a.Subject)
            .SingleAsync(i => i.Id == entity.Id);
        DeepAssert.Equal(entity, actualEntity, nameof(ActivityEntity.Subject));

    }
}