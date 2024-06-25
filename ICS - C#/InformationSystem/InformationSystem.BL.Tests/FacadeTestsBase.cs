using InformationSystem.BL.Mappers;
using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Factories;
using InformationSystem.DAL;
using InformationSystem.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using Xunit;
using Xunit.Abstractions;

namespace InformationSystem.BL.Tests;

public class FacadeTestsBase : IAsyncLifetime
{
    protected FacadeTestsBase(ITestOutputHelper output)
    {
        XUnitTestOutputConverter converter = new(output);
        Console.SetOut(converter);

        DbContextFactory = new DbContextSqLiteTestingFactory(GetType().FullName!, seedTestingData: true);

        ActivityModelMapper = new ActivityModelMapper();
        StudentEvaluationModelMapper = new StudentEvaluationModelMapper();
        StudentsInSubjectModelMapper = new StudentsInSubjectModelMapper();
        StudentModelMapper = new StudentModelMapper(StudentsInSubjectModelMapper, StudentEvaluationModelMapper);
        StudentsInSubjectModelMapper = new StudentsInSubjectModelMapper();
        SubjectModelMapper = new SubjectModelMapper(ActivityModelMapper);

        UnitOfWorkFactory = new UnitOfWorkFactory(DbContextFactory);
    }

    protected IDbContextFactory<InformationSystemDbContext> DbContextFactory { get; }

    protected ActivityModelMapper ActivityModelMapper { get; }
    protected StudentEvaluationModelMapper StudentEvaluationModelMapper { get; }
    protected StudentModelMapper StudentModelMapper { get; }
    protected StudentsInSubjectModelMapper StudentsInSubjectModelMapper { get; }
    protected SubjectModelMapper SubjectModelMapper { get; }
    protected UnitOfWorkFactory UnitOfWorkFactory { get; }

    public async Task InitializeAsync()
    {
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        await dbx.Database.EnsureDeletedAsync();
        await dbx.Database.EnsureCreatedAsync();
    }

    public async Task DisposeAsync()
    {
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        await dbx.Database.EnsureDeletedAsync();
    }
}
